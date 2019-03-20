#define   _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <sstream>  
#include <iterator>
#include <iomanip>
#include <windows.h>
#include <map>
#include <set>

using  namespace  std;

int PROBABILITY = 2;
int COUNTLINE = 0;
int iplus = 0;
double CONFI = 0.1;

void subset_enumerate(int *arr, int n, vector<string>& result)
{
	vector<string>().swap(result);
	char buf[32];
//	vector<string> temp;
	if (n == 1) {
		result.push_back(string(_itoa(arr[0], buf, 10)));
		return;
	}

	subset_enumerate(arr - 1, n - 1, result);

	vector<string> newss;
	string s(_itoa(arr[0], buf, 10));
	newss.push_back(s);
	for (int i = 0; i < result.size(); i++) {
		newss.push_back(result[i] + "," + s);
	}

	for (int i = 0; i < newss.size(); i++) {
		result.push_back(newss[i]);
	}
//	result.insert(result.end(), temp.begin(), temp.end());
}

void printall(map<int, vector<int>> &totalcount, vector<vector<int>> &totalname, map<vector<int>, int> &BackupNums)						//输出
{
	ofstream ofs("target.txt", ios::out|ios::app);
	int i = 0;	vector<string> result;
	for (auto & fqItemSet : totalname){
		for (int & item : fqItemSet)
			cout << item << " ";
		cout << ": " << totalcount[i++].size() << endl;
	}
	ofs << "产生规则！" << endl;
	for (int j = 0; j < totalname.size();j++)
	{
		map<int, vector<int>>::iterator itMap;
		itMap = totalcount.begin();
		for (int t = 0; t < j;t++)
		{
			++itMap;
		}
		int fenmu = itMap->second.size();
		subset_enumerate(&totalname[j][totalname[j].size()-1], totalname[j].size(), result);
		for (int i = 0; i < result.size(); i++) {
			/*cout << "{" << result[i] << "}" << endl;*/
			vector<int> temp; stringstream strStreamItem; string strItem;
			strStreamItem << result[i];
			while (getline(strStreamItem, strItem, ','))
			{
				temp.push_back(atoi(strItem.c_str()));
			}
			map<vector<int>, int>::iterator iter;
			for (iter = BackupNums.begin(); iter != BackupNums.end(); ++iter)
			{
				if (iter->first == temp)
				{
// 					vector<int>::iterator itertemp = find(temp.begin(),temp.end(),);
// 					if (itertemp == temp.end())
// 					{
// 						continue;
// 					}
					if ((double)fenmu / (double)iter->second >= CONFI && (double)fenmu / (double)iter->second != 1)
					{
						for (int & item : temp)
						{
							cout << item << " ";
							ofs << item << " ";
						}
							
						cout << " -> " << result[result.size() - i - 2];
						ofs << " -> " << result[result.size() - i - 2];
						cout << " : " << ((double)fenmu / (double)iter->second) << endl;
						ofs << " : " << ((double)fenmu / (double)iter->second) << endl;
					}
				}
			}
		}
	}
}


void printall(map<int, vector<int>> &totalcount, vector<vector<int>> &totalname, int timescount, DWORD TimeStart)						//输出
{
	ofstream ofs("target.txt", ios::out | ios::app);
	ofs << "频繁" << timescount << "-项集,时间" << GetTickCount() - TimeStart << "ms " << endl;
	int i = 0;
	map<int, vector<int>>::iterator iter;
	iter = totalcount.begin();
	for (auto & fqItemSet : totalname){
		for (auto & item : fqItemSet)
			ofs << item << " ";
		ofs << ": " << iter->second.size() << endl;
		++iter;
	}
}
//set
void addnewpoint(map<int, vector<int>> &FormName, set<int> &NameFirst, int strTid, int cntLine)
{
	map<int, vector<int>>::iterator iter;
	iter = FormName.find(strTid);
	if (iter != FormName.end())				//找到了
	{
		iter->second.push_back(cntLine);		//在对应的vector后面插入数据
	}
	else
	{
		vector<int> temp, tempfirst;					//没找到就新建一个
		temp.push_back(++iplus);
		tempfirst.push_back(cntLine);
		FormName.insert(pair<int, vector<int>>(strTid, tempfirst));
		NameFirst.insert(strTid);
	}
}
//set
void checksatisfication_prob(map<int, vector<int>> &FormName, set<int> &NameFirst, map<vector<int>, int> &BackupNums)						//删除
{
	map<int, vector<int>>::iterator itMap;
	set<int>::iterator itVec;
	vector<int> temp;
	for (itMap = FormName.begin(); itMap != FormName.end();)
	{
		vector<int>().swap(temp);
		if (itMap->second.size() < PROBABILITY)
		{
			itVec = NameFirst.find(itMap->first);
			NameFirst.erase(itVec);
			itMap = FormName.erase(itMap);
		}
		else
		{
			itVec = NameFirst.find(itMap->first);
			temp.push_back(*itVec);
			BackupNums.insert(pair<vector<int>, int>(temp, itMap->second.size()));
			++itMap;
		}
	}
}
//set
void loopforfinal(map<int, vector<int>> &FormName, map<int, vector<int>> &LatorName, map<int, vector<int>> &CalcName,
	set<int> &NameFirst, vector<vector<int>> &LatNameVec, vector<vector<int>> &FinNameVec, map<vector<int>, int> &BackupNums)
{
	if (LatorName.size() < LatNameVec[0].size()) return;

	map<int, vector<int>> tempmap, middlemap;
	vector<int> tempvec, tempnamevec;
	int countnum = 0, place = 0;
	map<int, vector<int>>::iterator itMap, itLatMap;

	for (itLatMap = LatorName.begin(); itLatMap != LatorName.end(); ++itLatMap)							//找出最终位置
	{
		//查找是不是末位函数
		itMap = FormName.find(LatNameVec[place][LatNameVec[place].size() - 1]);
		if (itMap == FormName.end())													//末位抛弃
		{
			++place;
			continue;
		}
		for (++itMap; itMap != FormName.end(); ++itMap)
		{
			int formlen = distance(FormName.begin(), itMap);
			vector<int>::iterator itFor, itLat;
			vector<int>().swap(tempvec);
			
			itFor = itMap->second.begin();
			itLat = itLatMap->second.begin();

			while (itFor != itMap->second.end() && itLat != itLatMap->second.end())
			{
				if (*itFor == *itLat)
				{
					tempvec.push_back(*itFor);
					++itFor;
					++itLat;
				}
				else
				{
					if (*itFor > *itLat)
					{
						++itLat;
					}
					else
					{
						++itFor;
					}
				}
			}
			if (tempvec.empty())
			{
				continue;
			}

			if (tempvec.size() >= PROBABILITY)
			{
				CalcName.insert(pair<int, vector<int>>(countnum, tempvec));
				tempnamevec = LatNameVec[place];
				set<int>::iterator itSet = NameFirst.find(itMap->first);
				tempnamevec.push_back(*itSet);
				FinNameVec.push_back(tempnamevec);
				BackupNums.insert(pair<vector<int>, int>(tempnamevec, tempvec.size()));
				++countnum;
			}
		}
		++place;
	}
}

void  main(int argv, char* argc[])
{
	string strname = "";
	cout << "请输入文件名" << endl;
	cin >> strname;
	cout << "请输入最小支持度" << endl;
	cin >> PROBABILITY;
	cout << "请输入最小置信度" << endl;
	cin >> CONFI;
	ofstream ofs("target.txt", ios::out);
	stringstream strStreamItem;
	ifstream  input = ifstream(strname);

	map<int, vector<int>> Cfirst, Ffinal;
	map<int, int> NameFirst;
	map<vector<int>, int> BackupNums;
	set<int> NameSet;
	vector<vector<int>> NameBackup, FinName;

	string strItem, tidtemp;
	int strTid = 0, counttime = 0;
	DWORD TimeStart = GetTickCount();
	DWORD TimeEnd, TotalTime;

	if (!input) //文件打开失败
	{
		cout << "存放事务的文件不存在，请先手工建立" << endl;
		return;
	}
	else
	{
		while (input)
		{
			string  str;
			getline(input, str, '\n');						//得到一行事务项集
			COUNTLINE++;									//tid计数
			strStreamItem.str("");
			strStreamItem.clear();
			strStreamItem << str;
			while (getline(strStreamItem, strItem, ' '))
			{ 
				addnewpoint(Cfirst, NameSet, atoi(strItem.c_str()), COUNTLINE);
			}
		}
	}
	PROBABILITY *= (COUNTLINE-1);
	PROBABILITY = (int)ceil(((double)PROBABILITY) / ((double)1000));

	checksatisfication_prob(Cfirst, NameSet, BackupNums);										//输出结果


	map<int, vector<int>> BackupFor(Cfirst);
	vector<vector<int>> LatName;
	set<int>::iterator itSet = NameSet.begin();
	for (itSet; itSet != NameSet.end(); ++itSet)
	{
		vector<int> temp;
		temp.push_back(*itSet);
		LatName.push_back(temp);
	}
	int timescount = 1;
	while (true)
	{
		if (counttime % 2)
		{
			cout << "频繁" << timescount << "-项集,时间" << GetTickCount() - TimeStart << endl;
			printall(Ffinal, FinName, timescount, TimeStart);
			loopforfinal(Cfirst, Ffinal, BackupFor, NameSet, FinName, LatName, BackupNums);
			timescount++;
		}
		else
		{
			cout << "频繁" << timescount << "-项集,时间" << GetTickCount() - TimeStart << endl;
			printall(BackupFor, LatName, timescount, TimeStart);
			loopforfinal(Cfirst, BackupFor, Ffinal, NameSet, LatName, FinName, BackupNums);
			timescount++;
		}
		if (Ffinal.empty() || BackupFor.empty())
		{
			TimeEnd = GetTickCount();
			TotalTime = TimeEnd - TimeStart;

			if (counttime % 2)
			{
				cout << "频繁" << FinName[0].size() << "-项集为：" << endl;
				printall(Ffinal, FinName, BackupNums);
			}
			else
			{
				cout << "频繁" << LatName[0].size() << "-项集为：" << endl;
				printall(BackupFor, LatName, BackupNums);
			}
			cout << "支持度为" << PROBABILITY << " ,共消耗时间" << TotalTime << "ms" << endl;
			break;
		}
		else
		{
			if (counttime % 2)
			{
				map<int, vector<int>>().swap(Ffinal);						//清空容器
				vector<vector<int>>().swap(FinName);						//清空容器
				counttime++;
			}
			else
			{
				map<int, vector<int>>().swap(BackupFor);					//清空容器
				vector<vector<int>>().swap(LatName);						//清空容器
				counttime++;
			}
		}
	}
	system("pause");
}
