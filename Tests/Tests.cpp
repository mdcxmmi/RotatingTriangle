#include "pch.h"
#include "CppUnitTest.h"
#include "../RotatingTriangle/RotatingTriangle.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	TEST_CLASS(Tests)
	{
	public:
		
		TEST_METHOD(changeAngileTest)
		{
			double angle = 25.0;
			double res = changeAngile(angle, 2.5);
			Assert::AreEqual(27.5, res);
		}
		TEST_METHOD(LineLengthTest)
		{
			POINT A = { 0.0,0.0 };
			POINT B = {25.0,0.0};

			Assert::AreEqual(LineLength(A,B), 25.0);
		}
		TEST_METHOD(InitTriangleTest)
		{
			InitTriangle();
			Assert::AreEqual(Triangle.thickness,(UCHAR)3);
		}
	};
}
