// Fill out your copyright notice in the Description page of Project Settings.

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

	virtual int32 GetNumOutputs() const override { return 1; }
	virtual FString GetFunctionName() const override { return TEXT("GetSFOutput"); }
	virtual FString GetDisplayName() const override { return TEXT("ShaderFlow Output"); }

#if WITH_EDITOR
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;
	
	virtual void GetCaption(TArray<FString>& OutCaptions) const override { OutCaptions.Add(FString(TEXT("SFOutput"))); }
	virtual uint32 GetInputType(int32 InputIndex) override;
	virtual FExpressionInput* GetInput(int32 InputIndex) override { return &Input; }
#endif
};
