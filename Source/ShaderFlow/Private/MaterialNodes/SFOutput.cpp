// Fill out your copyright notice in the Description page of Project Settings.


#include "MaterialNodes/SFOutput.h"

#include "MaterialCompiler.h"

#define LOCTEXT_NAMESPACE "USFOutput"

USFOutput::USFOutput(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		FText NAME_ShaderFlow;
		FConstructorStatics(const FString& DisplayName, const FString& FunctionName)
			: NAME_ShaderFlow(LOCTEXT("ShaderFlow", "ShaderFlow"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics(GetDisplayName(), GetFunctionName());

	MenuCategories.Add(ConstructorStatics.NAME_ShaderFlow);

	bCollapsed = true;
#endif
}

FString USFOutput::GetFunctionName() const
{
	if (!Description.IsEmpty())
		return FString(TEXT("Get")) + Description;
	return TEXT("GetSFOutput");
}

#if WITH_EDITOR
int32 USFOutput::Compile(class FMaterialCompiler* Compiler, int32 OutputIndex)
{
	if (Input.GetTracedInput().Expression)
	{
		return Compiler->CustomOutput(this, OutputIndex, Input.Compile(Compiler));
	}
	
	return CompilerError(Compiler, TEXT("Input missing"));
}

void USFOutput::GetCaption(TArray<FString>& OutCaptions) const
{
	if (Description.IsEmpty())
		OutCaptions.Add(FString(TEXT("SFOutput")));
	else
		OutCaptions.Add(Description);		
}

uint32 USFOutput::GetInputType(int32 InputIndex)
{
	if (Input.GetTracedInput().Expression)
		return Input.GetTracedInput().Expression->GetOutputType(Input.GetTracedInput().OutputIndex);
	return MCT_Unknown;
}
#endif

#undef LOCTEXT_NAMESPACE