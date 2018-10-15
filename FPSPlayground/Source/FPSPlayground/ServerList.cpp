// Fill out your copyright notice in the Description page of Project Settings.

#include "ServerList.h"
#include "Components/Button.h"
#include "MainMenu.h"




void UServerList::Setup(class UMainMenu* InParent, uint32 InIndex)
{
	Parent = InParent;
	Index = InIndex;
	ListButton->OnClicked.AddDynamic(this, &UServerList::OnClicked);
}

void UServerList::OnClicked()
{
	Parent->SelectIndex(Index);
}