#pragma once
#include <array>
#include <cmath>
#include <stack>



template <typename T, std::size_t size>
class ProbabilityTable
{
public:
	ProbabilityTable(std::array<T, size> _outcomes, std::array<double, size> _probabilities) :
		outcomes{_outcomes}
	{
		double _average = 1.0 / size;
		std::stack<std::size_t> small;
		std::stack<std::size_t> large;
		for (std::size_t i = 0; i < size; ++i) {
			if (_probabilities[i] >= _average) large.push(i);
			else small.push(i);
		}
		while (!small.empty() && !large.empty())
		{
			auto _small = small.top();
			small.pop();
			auto _large = large.top();
			large.pop();

			probability[_small] = _probabilities[_small] * size;
			alias[_small] = _large;

			_probabilities[_large] = (_probabilities[_small] + _probabilities[_large]) - _average;

			if (_probabilities[_large] >= _average) large.push(_large);
			else small.push(_large);
		}
		while (!large.empty())
		{
			probability[large.top()] = 1.0;
			large.pop();
		}
		while (!small.empty())
		{
			probability[small.top()] = 1.0;
			small.pop();
		}
	}



	T getOutcome(double _rand1, double _rand2) const
	{
		std::size_t _index = std::min(static_cast<std::size_t>(std::floor(_rand1 * size)), size);
		return outcomes[_rand2 < probability[_index] ? _index : alias[_index]];
	}

private:
	std::array<double, size> probability;
	std::array<std::size_t, size> alias;
	std::array<T, size> outcomes;
};