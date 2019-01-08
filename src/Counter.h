#pragma once




class Counter
{
public:
	void Start();

	int GetCount() const;

	void Increment();

private:
	int mValue = 0;

};

