#include <iostream>
#include <array>
#include <amp.h>

void StandardMethod(void)
{
	// Copied from MSDN web site, http://msdn.microsoft.com/en-us/library/windows/apps/hh265136.aspx

	// Initialization.
	int a[] = { 1, 2, 3, 4, 5 };
	int b[] = { 6, 7, 8, 9, 10 };
	int c[5];

	// Computation.
	for (int idx = 0; idx < 5; idx++)
		c[idx] = a[idx] + b[idx];

	// Checking.
	for (int idx = 0; idx < 5; idx++)
		std::cout << c[idx] << std::endl;
	std::cout << std::endl;
}

void CpuOnly(void)
{
	std::cout << "CpuOnly" << std::endl;

	// Modified StandardMethod() to more C++ way.

	// Initialization.
	const int SIZE(5);
	std::array<int, SIZE> a = { 1, 2, 3, 4, 5 }, b = { 6, 7, 8, 9, 10 }, c;

	// Computation.
	auto it_a = a.cbegin(), it_b = b.cbegin();
	for (auto it_c = c.begin(); it_c != c.end(); ++it_a, ++it_b, ++it_c)
		*it_c = *it_a + *it_b;

	// Checking.
	for (auto it_c = c.cbegin(); it_c != c.cend(); ++it_c)
		std::cout << *it_c << std::endl;
	std::cout << std::endl;
}

void CppAmpMethod(void)
{
	// Copied from MSDN web site, http://msdn.microsoft.com/en-us/library/windows/apps/hh265136.aspx

	// Initialization.
	const int SIZE(5);
	int a[] = { 1, 2, 3, 4, 5 };
	int b[] = { 6, 7, 8, 9, 10 };
	int c[SIZE];

	// Create C++ AMP objects.
	concurrency::array_view<const int, 1> a_amp(SIZE, a);
	concurrency::array_view<const int, 1> b_amp(SIZE, b);
	concurrency::array_view<int, 1> c_amp(SIZE, c);
	c_amp.discard_data();

	// Computation.
	parallel_for_each(
		c_amp.extent,
		[=](concurrency::index<1> idx) restrict(amp)
	{
		c_amp[idx] = a_amp[idx] + b_amp[idx];
	}
	);

	// Checking interim result.
	for (int i = 0; i < 5; i++)
		std::cout << c_amp[i] << std::endl;

	// Checking final output.
	for (int i(0); i < 5; ++i)
		std::cout << c[i] << std::endl;		// Working.

	std::cout << std::endl;
}

// Recommended.
void UseAmp(void)
{
	std::cout << "UseAmp" << std::endl;

	// Modified CppAmpMethod() to more C++ way.

	// Initialization.
	const int SIZE(5);
	std::array<int, SIZE> a = { 1, 2, 3, 4, 5 }, b = { 6, 7, 8, 9, 10 }, c;

	// Create C++ AMP objects.
	concurrency::array_view<const int, 1> a_amp(SIZE, a), b_amp(SIZE, b);
	concurrency::array_view<int, 1> c_amp(SIZE, c);

	// Computation.
	concurrency::parallel_for_each(
		c_amp.extent,
		[=](concurrency::index<1> idx) restrict(amp)
	{
		c_amp[idx] = a_amp[idx] + b_amp[idx];
	}
	);

	// Checking interim result.
	for (int i(0); i < 5; ++i)
		std::cout << c_amp[i] << std::endl;

	// Checking final output.
	for (auto it_c = c.cbegin(), end = c.cend(); it_c != end; ++it_c)
		std::cout << *it_c << std::endl;	// Working.

	std::cout << std::endl;
}

void AddElements(concurrency::index<1> idx, const concurrency::array_view<const int, 1> &a, const concurrency::array_view<const int, 1> &b, concurrency::array_view<int, 1> c) restrict(amp)
{
	c[idx] = a[idx] + b[idx];
}

void UseAmp_2(void)
{
	std::cout << "UseAmp_2" << std::endl;

	// Modified UseAmp().

	// Initialization.
	const int SIZE(5);
	std::array<int, SIZE> a = { 1, 2, 3, 4, 5 }, b = { 6, 7, 8, 9, 10 }, c;

	// Create C++ AMP objects.
	concurrency::array_view<const int, 1> a_amp(SIZE, a), b_amp(SIZE, b);
	concurrency::array_view<int, 1> c_amp(SIZE, c);

	// Computation.
	concurrency::parallel_for_each(
		c_amp.extent,
		[=](concurrency::index<1> idx) restrict(amp)
	{
		AddElements(idx, a_amp, b_amp, c_amp);
	}
	);

	// Checking interim result.
	for (int i(0); i < 5; ++i)
		std::cout << c_amp[i] << std::endl;

	// Checking final output.
	for (auto it_c = c.cbegin(), end = c.cend(); it_c != end; ++it_c)
		std::cout << *it_c << std::endl;	// Working.

	std::cout << std::endl;
}


void UseArray(void)
{
	// Initialization.
	const int SIZE(5);
	std::vector<int> a(SIZE);
	for (auto i(0); i != a.size(); ++i)
		//for (auto elem : data)
		a[i] = i;

	// Computation.
	concurrency::array<int, 1> a_amp(SIZE, a.begin(), a.end());
	concurrency::parallel_for_each(
		a_amp.extent,
		[=, &a_amp](concurrency::index<1> idx) restrict(amp)
	{
		a_amp[idx] = a_amp[idx] * 10;
	}
	);

	a = a_amp;	// Must copy the data back to the source.
	for (auto it = a.begin(); it != a.end(); ++it)
		std::cout << *it << std::endl;	// Working.
}

int main(void)
{
	StandardMethod();
	CpuOnly();
	CppAmpMethod();
	UseAmp();
	UseAmp_2();

	UseArray();
}
