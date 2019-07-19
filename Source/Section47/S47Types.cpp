// Fill out your copyright notice in the Description page of Project Settings.

#include "S47Types.h"


ETeamAttitude::Type S47TeamAttitudeSolver(FGenericTeamId A, FGenericTeamId B)
{
	return A.GetId() != B.GetId() ? ETeamAttitude::Hostile : ETeamAttitude::Friendly;
}
