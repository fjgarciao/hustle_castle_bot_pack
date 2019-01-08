#pragma once

#include "pch.h"
#include "IArenaAlgorithm.h"


class ArenaAlgorithm1 : public IArenaAlgorithm
{
public:
	virtual int FindOpponent(const PowerData &data, int phase) override;

	// для первой фазы: если слабых много - бьем сильных из слабых. если слабых мало - бьем самых слабых.
	// ищем среднюю мощность всех  кто слабее и кого можем убить на текущей фазе
	// чем меньше средняя мощность, тем более сильного противника из этого списка выбираем.
	// ищем противников, которые не могут набрать 45 очков.
	//

private:
	int FindMe(const PowerData &data);

	// Получить среднюю мощность противников.
	int GetWeakMeanPower(const std::vector<ArenaPowerInfo> &opps);

	// Список всех противников по увеличению мощности
	std::vector<ArenaPowerInfo> GetAll(const PowerData &data);

	// Список слабых противников ниже меня, которые не могут набрать высокую ставку, которых можно убить на данный момент.
	std::vector<ArenaPowerInfo> GetBelowWeakCannot(const PowerData &data, int count = 0);

	// Список слабых противников выше меня, которые не могут набрать высокую ставку, которых можно убить на данный момент.
	std::vector<ArenaPowerInfo> GetAboveWeakCannot(const PowerData &data, int count = 0);

	// Список слабых противников выше меня, которые могут убить кого то выше себя.
	std::vector<ArenaPowerInfo> GetAboveWeakCan(const PowerData &data);

	// Список слабых противников ниже меня, которых можно убить на данный момент.
	std::vector<ArenaPowerInfo> GetBelowWeak(const PowerData &data);

	// Список слабых противников выше меня, которых можно убить на данный момент.
	std::vector<ArenaPowerInfo> GetAboveWeak(const PowerData &data);

	// Список всех слабых противников, которых можно убить на данный момент.
	std::vector<ArenaPowerInfo> GetWeak(const PowerData &data);

	// количество слабых противников выше
	int CountWeatAbove(const PowerData &data, int pos);

	int FindAdaptivePos(const std::vector<ArenaPowerInfo> &opps, bool &reserve_choise);
	
	void IgnoreMeanPower(const PowerData &data, std::vector<ArenaPowerInfo> &power, int count);
};

