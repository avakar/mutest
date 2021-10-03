#include "mutest.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

namespace avakar {
namespace mutest {

_test_registrar * _test_registry = nullptr;

int run(int argc, char const * const * argv)
{
	size_t pre_context = 4;
	size_t post_context = 2;

	bool success = true;
	_test_registrar * cur = _test_registry;
	while (cur)
	{
		try
		{
			cur->fn();
		}
		catch (check_failed const & e)
		{
			std::vector<std::string> line_ring(pre_context + post_context + 1);
			std::size_t ring_pos = 0;

			size_t target_lineno = e.line();
			size_t lineno = 1;
			{
				std::ifstream fin(e.file());
				while (std::getline(fin, line_ring[ring_pos]))
				{
					if (++ring_pos == line_ring.size())
						ring_pos = 0;
					if (lineno++ == target_lineno + post_context)
						break;
				}
			}

			size_t w = 0;
			for (int i = lineno; i > 0; i /= 10)
				++w;

			std::cout << e.file() << "(" << target_lineno << "): error MT0: " << cur->name << "\n";

			size_t first_context_line = lineno;
			if (target_lineno >= 1 && target_lineno < lineno)
			{
				if (target_lineno - 1 >= pre_context)
					first_context_line = target_lineno - pre_context;
				else
					first_context_line = 1;
			}

			ring_pos = ring_pos + (line_ring.size() - (lineno - first_context_line));
			if (ring_pos >= line_ring.size())
				ring_pos -= line_ring.size();

			for (int i = first_context_line; i != lineno; ++i)
			{
				char const * marker = i == e.line()? " >": "  ";
				auto const & line = line_ring[ring_pos];

				if (line.empty())
				{
					std::cout << marker << std::setw(w+1) << i << "\n";
				}
				else
				{
					std::cout << marker << std::setw(w+1) << i << " ";

					size_t initial_tab_count = 0;
					while (initial_tab_count != line.size() && line[initial_tab_count] == '\t')
					{
						std::cout << "    ";
						++initial_tab_count;
					}

					std::cout << line.c_str() + initial_tab_count << "\n";
				}

				if (++ring_pos == line_ring.size())
					ring_pos = 0;
			}
			success = false;
		}
		catch (std::exception const & e)
		{
			return 3;
		}
		catch (...)
		{
			return 4;
		}

		cur = cur->next;
	}

	return success? 0: 1;
}

}
}

#pragma once
