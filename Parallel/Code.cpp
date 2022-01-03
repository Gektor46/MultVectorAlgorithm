#include <iostream>
#include <execution>
#include <vector>
#include <thread>
#include <random>
#include <iterator>

//Таймер
class SimpleTimer
{
public:
	SimpleTimer()
	{
		Start = std::chrono::high_resolution_clock::now();
	}
	~SimpleTimer()
	{
		End = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> Duration = End - Start;
		std::cout << "Продолжительность работы: " << Duration.count() << " (секунд)" << std::endl;
	}
private:
	std::chrono::time_point<std::chrono::steady_clock> Start, End;
};

//Заполняет вектор случайными числами типа (double)
std::vector<double> RandomVector(size_t size)
{
	std::vector<double> v(size);
	std::random_device r;
	std::generate(v.begin(), v.end(), [&] {return r(); });
	return v;
}

//Перемножить элементы двух векторов с помощью потоков
void MultVector(std::vector<double> &Vec1, std::vector<double> &Vec2, std::vector<double> &Vec3, unsigned Thr, unsigned ThrVec)
{
	unsigned Index = Thr; 
	for (unsigned iv = 0; iv < ThrVec; ++iv)
	{
		Vec3[Index] = Vec1[Index] * Vec2[Index];
		++Index;
	}
}

//Создать потоки для перемножения векторов
void ThreadVector(std::vector<double> &Vec1, std::vector<double> &Vec2, size_t SizeVec, size_t SizeThr)
{
	SimpleTimer Timer;

	std::vector<double> Vec3(SizeVec);
	std::vector<std::thread> Threads;

	unsigned Thr = 0;
	unsigned ThrVec = SizeVec / SizeThr;
	unsigned ThrVecRem = SizeVec % SizeThr;

	for (unsigned it = 0; it < SizeThr; ++it)
	{
		if (ThrVecRem != 0)
		{
			Threads.emplace_back(MultVector, std::ref(Vec1), std::ref(Vec2), std::ref(Vec3), Thr, ThrVec + 1);
			ThrVecRem--;
		}
		else
		{
			Threads.emplace_back(MultVector, std::ref(Vec1), std::ref(Vec2), std::ref(Vec3), Thr, ThrVec);
		}
		Thr = Thr + ThrVec;
	}
	for (auto& entry : Threads)
		entry.join();
}

//Перемножить элементы двух векторов с помощью алгоритма
void MultVectorAlgorithm(std::vector<double> &Vec1, std::vector<double> &Vec2, size_t SizeVec)
{
	std::cout << "***С помощью алгоритма***" << std::endl;
	SimpleTimer Timer;

	std::vector<double> Vec3(SizeVec);

	std::transform(std::execution::par, Vec1.begin(), Vec1.end(), Vec2.begin(), Vec3.begin(), std::multiplies<double>());
}

int main()
{
	setlocale(LC_ALL, "ru");

	size_t SizeVec;
	std::cout << "Размер векторов: ";
	std::cin >> SizeVec;

	size_t SizeThr;
	std::cout << "Количество потоков: ";
	std::cin >> SizeThr;

	if (SizeThr > SizeVec)
	{
		SizeThr = SizeVec;
		std::cout << "Есть лишние потоки, количество потоков сокращено до: " << SizeThr << std::endl;
	}

	std::vector<double> Vec1 = RandomVector(SizeVec);
	std::vector<double> Vec2 = RandomVector(SizeVec);
	
	ThreadVector(std::ref(Vec1), std::ref(Vec2), SizeVec, SizeThr);

	MultVectorAlgorithm(std::ref(Vec1), std::ref(Vec2), SizeVec);

	system("pause");
	return 0;
}