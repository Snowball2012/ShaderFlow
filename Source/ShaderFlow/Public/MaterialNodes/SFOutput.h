// Copyright(c) 2021 Sergey Kulikov

#pragma once

#include "CoreMinimal.h"
#include "Materials/MaterialExpressionCustomOutput.h"
#include "SFOutput.generated.h"

/**
 * 
 */
UCLASS()
class SHADERFLOW_API USFOutput : public UMaterialExpressionCustomOutput
{
	GENERATED_UCLASS_BODY()
	
	UPROPERTY(meta = (RequiredInput = "true"))
	FExpressionInput Input;

	UPROPERTY(EditAnywhere, Category=SFOutput)
	FString Description;

	virtual int32 GetNumOutputs() const override { return 1; }
	virtual FString GetFunctionName() const override;
	virtual FString GetDisplayName() const override { return TEXT("ShaderFlow Output"); }
	
#if WITH_EDITOR	
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual bool AllowMultipleCustomOutputs() override { return true; }	
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
	virtual uint32 GetInputType(int32 InputIndex) override;
	virtual FExpressionInput* GetInput(int32 InputIndex) override { return &Input; }
#endif
};
