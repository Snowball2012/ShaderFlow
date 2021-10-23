// Copyright(c) 2021 Sergey Kulikov

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
	const FString Base = TEXT("GetSFOutput_");

	const FString GuidStr = MaterialExpressionGuid.ToString(EGuidFormats::Base36Encoded);
	
	// Unique hash to differentiate between the same node called in different material functions
	const uint32 FunctionHash = GetTypeHash(Function ? Function->GetFullName() : FString());
	
	// Theoretically we don't really need ObjHash because Guid should be enough.
	// However, copy-pasting a node preserves its GUID, so it's better to add an extra hash
	const uint32 ObjHash = GetTypeHash(GetFullName());
	
	const uint32 Hash = HashCombine(ObjHash, FunctionHash);

	const FString HashStr = FString::FromHexBlob((uint8*)&Hash, sizeof(uint32));
	
	return Base	+ GuidStr + HashStr;
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