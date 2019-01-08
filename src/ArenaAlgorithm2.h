#pragma once

#include "pch.h"
#include "IArenaAlgorithm.h"


class ArenaAlgorithm2 : public IArenaAlgorithm
{
public:
	virtual int FindOpponent(const PowerData &data, int phase) override;


private:
	// фаза 1 и фаза 2. убиваем самых слабых ниже меня.


	int FindMe(const PowerData &data);



	// Список слабых противников выше меня, которые могут убить кого то выше себя.
	std::vector<ArenaPowerInfo> GetAboveWeakCan(const PowerData &data);

	// Список слабых противников выше меня, которые не могут убить кого то выше себя.
	std::vector<ArenaPowerInfo> GetAboveWeakCannot(const PowerData &data);

	// Список слабых противников выше меня, которых можно убить на данный момент.
	std::vector<ArenaPowerInfo> GetAboveWeak(const PowerData &data);

	// количество слабых противников выше
	int CountWeatAbove(const PowerData &data, int pos);

	// Список слабых противников ниже меня, которых можно убить на данный момент.
	std::vector<ArenaPowerInfo> GetBelowWeak(const PowerData &data);

	// Ищем тех, кто ниже меня и не могут убить 2 вверх.
	std::vector<ArenaPowerInfo> GetPhase3(PowerData data);

	// Получить всех кого можно убить.
	std::vector<ArenaPowerInfo> GetAll(const PowerData &data);

	// Список всех слабых противников, которых можно убить на данный момент.
	std::vector<ArenaPowerInfo> GetWeak(const PowerData &data);
};

