// Fill out your copyright notice in the Description page of Project Settings.


#include "ToolFunc.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

bool UToolFunc::LoadTxt(FString FileNameA, TArray<FString>& Result)
{
   string loadPath(TCHAR_TO_UTF8(*(FPaths::ProjectDir() + FileNameA)));

   ifstream inFile;
   inFile.open(loadPath, ios_base::in);

   if (!inFile.is_open())
      return false;

   while (!inFile.eof())
   {
      string str;
      getline(inFile, str);
      if (str != "")
      {
         FString fstr(str.c_str());
         Result.Push(fstr);
      }
   }

   inFile.close();

   return true;
}

bool UToolFunc::SaveTxt(FString SaveTextB, FString FileNameB)
{
   string savetext(TCHAR_TO_UTF8(*SaveTextB));
   string savePath(TCHAR_TO_UTF8(*(FPaths::ProjectDir() + FileNameB)));

   ofstream outFile;
   outFile.open(savePath, ios_base::out | ios_base::app);

   if (!outFile.is_open())
      return false;

   outFile << savetext << endl;

   outFile.close();

   return true;
}
