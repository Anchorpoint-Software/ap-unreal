// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <EditorSubsystem.h>

#include "AnchorpointCliConnectSubsystem.generated.h"

class FInteractiveProcess;

USTRUCT()
struct FAnchorpointConnectMessage 
{
	GENERATED_BODY()


	UPROPERTY()
	FString Id;

	UPROPERTY()
	FString Type;

	UPROPERTY()
	TArray<FString> Files;
};

USTRUCT()
struct FAnchorpointCliConnectResponse 
{
	GENERATED_BODY()

	UPROPERTY()
	FString Id;

	UPROPERTY()
	FString Error;
};

/**
 * 
 */
UCLASS()
class ANCHORPOINTCLI_API UAnchorpointCliConnectSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

	//~ Begin UEditorSubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	//~ End UEditorSubsystem Interface

	void HandleMessage(const FAnchorpointConnectMessage& Message);
	void RespondToMessage(const FString& Id, TOptional<FString> Error = TOptional<FString>());
	
	bool UnlinkObjects(const TArray<FString>& InFiles);
	bool RelinkObjects(const TArray<FString>& InFiles);

	void OnOutput(const FString& Output);
	void OnCompleted(int ReturnCode, bool bCanceling);
	void OnCanceled();

	UPROPERTY()
	TArray<UPackage*> UnlinkedObjects;

	TSharedPtr<FInteractiveProcess> Process = nullptr;
};
