// Copyright(c) 2021 Sergey Kulikov

#include "MaterialNodes/SFDynamicIfExpression.h"

#include "MaterialCompiler.h"
#include "MaterialNodes/SFOutput.h"
#include "Materials/MaterialExpressionCustom.h"

#define LOCTEXT_NAMESPACE "USFDynamicIfExpression"

USFDynamicIfExpression::USFDynamicIfExpression(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		FText NAME_ShaderFlow;
		FConstructorStatics()
			: NAME_ShaderFlow(LOCTEXT("ShaderFlow", "ShaderFlow"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	ForwardDeclarationExpression = CreateDefaultSubobject<UMaterialExpressionCustom>(TEXT("ForwardDeclarationExpression"));
	BranchExpression = CreateDefaultSubobject<UMaterialExpressionCustom>(TEXT("BranchExpression"));

#if WITH_EDITORONLY_DATA
	MenuCategories.Add(ConstructorStatics.NAME_ShaderFlow);
#endif
}

#if WITH_EDITOR
int32 USFDynamicIfExpression::Compile(FMaterialCompiler* Compiler, int32 OutputIndex)
{
	// 1. Collect SFOutputs and compile regular inputs
	if (!Condition.GetTracedInput().Expression)
	{
		return Compiler->Errorf(TEXT("Missing Condition input"));
	}
	if (!TrueExpr.GetTracedInput().Expression)
	{
		return Compiler->Errorf(TEXT("Missing True input"));
	}
	if (!FalseExpr.GetTracedInput().Expression)
	{
		return Compiler->Errorf(TEXT("Missing False input"));
	}

	const auto* TrueSFOutput = Cast<USFOutput>(TrueExpr.GetTracedInput().Expression);
	const auto* FalseSFOutput = Cast<USFOutput>(FalseExpr.GetTracedInput().Expression);

	const bool HasSFInputs = bool(TrueSFOutput) || bool(FalseSFOutput);
	
	if (HasSFInputs)
	{
		FString SFOutputTypeDecl = TEXT("FMaterialAttributes ");
		switch (OutputType)
		{
		case CMOT_Float1:
			SFOutputTypeDecl = TEXT("MaterialFloat ");
			break;
		case CMOT_Float2:
			SFOutputTypeDecl = TEXT("MaterialFloat2 ");
			break;
		case CMOT_Float3:
			SFOutputTypeDecl = TEXT("MaterialFloat3 ");
			break;
		case CMOT_Float4:
			SFOutputTypeDecl = TEXT("MaterialFloat4 ");
			break;
		}
		if (!ensure(ForwardDeclarationExpression))
			return INDEX_NONE;

		ForwardDeclarationExpression->Inputs.Empty();
		ForwardDeclarationExpression->Inputs.AddDefaulted();
		auto& FwdInput = ForwardDeclarationExpression->Inputs[0];
		FwdInput.Input = Condition.GetTracedInput();
		FwdInput.InputName = TEXT("Condition");

		ForwardDeclarationExpression->OutputType = CMOT_Float1;

		auto& FwdCode = ForwardDeclarationExpression->Code;
		FwdCode.Empty();
		FwdCode.Append(TEXT("return Condition;\n}\n"));
		if (TrueSFOutput)
		{
			FwdCode.Append(SFOutputTypeDecl);
			FwdCode.Append(TrueSFOutput->GetFunctionName());
			FwdCode.AppendInt(0);
			FwdCode.Append(TEXT("(FMaterialPixelParameters Parameters); \n"));
		}
		if (FalseSFOutput)
		{
			FwdCode.Append(SFOutputTypeDecl);
			FwdCode.Append(FalseSFOutput->GetFunctionName());
			FwdCode.AppendInt(0);
			FwdCode.Append(TEXT("(FMaterialPixelParameters Parameters); \n"));
		}
		FwdCode.Append(TEXT("#define SHADERFLOW_DUMMY_DEFINE { \\"));
	}

	// Branch
	if (!ensure(BranchExpression))
		return INDEX_NONE;

	{
		BranchExpression->OutputType = OutputType;
		
		BranchExpression->Inputs.Empty();
		auto& BranchExprConditionInput = BranchExpression->Inputs.AddDefaulted_GetRef();
		BranchExprConditionInput.InputName = TEXT("Condition");
		if (HasSFInputs)
		{
			BranchExprConditionInput.Input.Connect(0, ForwardDeclarationExpression);
		}
		else
		{
			BranchExprConditionInput.Input = Condition.GetTracedInput();
		}
		if (!TrueSFOutput)
		{
			auto& BranchExprTrueInput = BranchExpression->Inputs.AddDefaulted_GetRef();
			BranchExprTrueInput.InputName = TEXT("TrueExpr");
			BranchExprTrueInput.Input = TrueExpr.GetTracedInput();
		}
		if (!FalseSFOutput)
		{
			auto& BranchExprFalseInput = BranchExpression->Inputs.AddDefaulted_GetRef();
			BranchExprFalseInput.InputName = TEXT("FalseExpr");
			BranchExprFalseInput.Input = FalseExpr.GetTracedInput();
		}
		
		auto& BranchCode = BranchExpression->Code;
		BranchCode.Empty();
		BranchCode.Append(TEXT("    [branch] if (Condition)\n"));
		BranchCode.Append(TEXT("    {\n"));
		BranchCode.Append(TEXT("        return "));
		if (TrueSFOutput)
		{
			BranchCode.Append(TrueSFOutput->GetFunctionName());
			BranchCode.AppendInt(0);
			BranchCode.Append(TEXT("(Parameters)"));
		}
		else
		{
			BranchCode.Append("TrueExpr");
		}
		BranchCode.Append(TEXT(";\n    }\n    return "));
		if (FalseSFOutput)
		{
			BranchCode.Append(FalseSFOutput->GetFunctionName());
			BranchCode.AppendInt(0);
			BranchCode.Append(TEXT("(Parameters)"));
		}
		else
		{
			BranchCode.Append("FalseExpr");
		}
		BranchCode.Append(TEXT(";\n"));
	}
	
	return BranchExpression->Compile(Compiler, 0);
}

void USFDynamicIfExpression::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add(TEXT("SFDynamicIf"));
}

uint32 USFDynamicIfExpression::GetInputType(int32 InputIndex)
{
	return MCT_Unknown;
}

bool USFDynamicIfExpression::IsResultMaterialAttributes(int32 OutputIndex)
{
	return TrueExpr.GetTracedInput().Expression && !TrueExpr.Expression->ContainsInputLoop() && TrueExpr.Expression->IsResultMaterialAttributes(TrueExpr.OutputIndex)
	&& FalseExpr.GetTracedInput().Expression && !FalseExpr.Expression->ContainsInputLoop() && FalseExpr.Expression->IsResultMaterialAttributes(FalseExpr.OutputIndex);
}
#endif

#undef LOCTEXT_NAMESPACE