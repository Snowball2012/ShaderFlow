// Copyright(c) 2021 Sergey Kulikov

#pragma once

#include "CoreMinimal.h"
#include "MaterialExpressionIO.h"
#include "Materials/MaterialExpression.h"
#include "Materials/MaterialExpressionCustom.h"
#include "SFDynamicIfExpression.generated.h"

class UMaterialExpressionCustom;
/**
 * 
 */
UCLASS(collapsecategories, hidecategories=Object)
class SHADERFLOW_API USFDynamicIfExpression : public UMaterialExpression
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(meta = (RequiredInput = "true"))
	FExpressionInput Condition;

	UPROPERTY(meta = (RequiredInput = "true"))
	FExpressionInput TrueExpr;

	UPROPERTY(meta = (RequiredInput = "true"))
	FExpressionInput FalseExpr;

	UPROPERTY(EditAnywhere, Category=SFOutput)
	TEnumAsByte<ECustomMaterialOutputType> OutputType = CMOT_Float1;

	// Forward declare functions from connected True/False pins
	UPROPERTY()
	UMaterialExpressionCustom* ForwardDeclarationExpression;

	// Real branch node
	UPROPERTY()
	UMaterialExpressionCustom* BranchExpression;

#if WITH_EDITOR
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
	virtual uint32 GetInputType(int32 InputIndex) override;
	virtual uint32 GetOutputType(int32 InputIndex) override {return MCT_Unknown;}
	virtual bool IsResultMaterialAttributes(int32 OutputIndex) override;
#endif
};
