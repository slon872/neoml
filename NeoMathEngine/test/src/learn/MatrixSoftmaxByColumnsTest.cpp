/* Copyright © 2017-2024 ABBYY

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
--------------------------------------------------------------------------------------------------------------*/

#include <TestFixture.h>
#include <MeTestCommon.h>

using namespace NeoML;
using namespace NeoMLTest;

static void matrixSoftmaxByColumnsTestImpl( const CTestParams& params, int seed )
{
	CRandom random( seed );

	const CInterval heightInterval = params.GetInterval( "Height" );
	const CInterval widthInterval = params.GetInterval( "Width" );
	const CInterval valuesInterval = params.GetInterval( "Values" );

	const int height = random.UniformInt( heightInterval.Begin, heightInterval.End );
	const int width = random.UniformInt( widthInterval.Begin, widthInterval.End );

	CREATE_FILL_FLOAT_ARRAY( matrix, valuesInterval.Begin, valuesInterval.End, height * width, random )
	std::vector<float> get, expected;
	get.insert( get.begin(), height * width, 0 );
	expected.insert( expected.begin(), height * width, 0 );

	MathEngine().MatrixSoftmaxByColumns( CARRAY_FLOAT_WRAPPER( matrix ), height, width, CARRAY_FLOAT_WRAPPER( get ) );

	softmaxImpl( matrix, height, width, false, expected );

	for( int i = 0; i < height; ++i ) {
		EXPECT_NEAR( expected[i], get[i], 1e-3 );
	}
}

//---------------------------------------------------------------------------------------------------------------------

class CMatrixSoftmaxByColumnsTest : public CTestFixtureWithParams {
};

INSTANTIATE_TEST_CASE_P( CMatrixSoftmaxByColumnsTestInstantiation, CMatrixSoftmaxByColumnsTest,
	::testing::Values(
		CTestParams(
			"Height = (1..50);"
			"Width = (1..50);"
			"Values = (-1..1);"
			"TestCount = 100;"
		),
		CTestParams(
			"Height = (100..500);"
			"Width = (100..500);"
			"Values = (-1..1);"
			"TestCount = 5;"
		)
	)
);

TEST_P( CMatrixSoftmaxByColumnsTest, Random )
{
	const auto met = MathEngine().GetType();
	if(met != MET_Cpu && met != MET_Cuda) {
		NEOML_HILIGHT( GTEST_LOG_( INFO ) ) << "Skip for MathEngine type= " << met << " , investigate later.\n";
		return;
	}

	RUN_TEST_IMPL( matrixSoftmaxByColumnsTestImpl )
}
