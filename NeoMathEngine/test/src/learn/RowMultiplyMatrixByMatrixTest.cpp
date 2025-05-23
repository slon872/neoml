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

using namespace NeoML;
using namespace NeoMLTest;

static void rowMultiplyMatrixByMatrixTestImpl( const CTestParams& params, int seed )
{
	CRandom random( seed );

	const CInterval widthInterval = params.GetInterval( "Width" );
	const CInterval heightInterval = params.GetInterval( "Height" );
	const CInterval valuesInterval = params.GetInterval( "Values" );

	const int height = random.UniformInt( heightInterval.Begin, heightInterval.End );
	const int width = random.UniformInt( widthInterval.Begin, widthInterval.End );

	CREATE_FILL_FLOAT_ARRAY( firstData, valuesInterval.Begin, valuesInterval.End, height * width, random )
	CREATE_FILL_FLOAT_ARRAY( secondData, valuesInterval.Begin, valuesInterval.End, height * width, random )

	std::vector<float> expected, get;
	expected.insert( expected.begin(), height, 0.f );
	get.resize( height );

	for( int i = 0; i < height; ++i ) {
		for( int j = 0; j < width; ++j ) {
			expected[i] += firstData[i * width + j] * secondData[i * width + j];
		}
	}

	MathEngine().RowMultiplyMatrixByMatrix( CARRAY_FLOAT_WRAPPER( firstData ), CARRAY_FLOAT_WRAPPER( secondData ),
		height, width, CARRAY_FLOAT_WRAPPER( get ) );

	for( int i = 0; i < height; ++i ) {
		EXPECT_NEAR( expected[i], get[i], 1e-3 );
	}
}

//---------------------------------------------------------------------------------------------------------------------

class CRowMultiplyMatrixByMatrixTest : public CTestFixtureWithParams {
};

INSTANTIATE_TEST_CASE_P( CRowMultiplyMatrixByMatrixTestInstantiation, CRowMultiplyMatrixByMatrixTest,
	::testing::Values(
		CTestParams(
			"Height = (1..50);"
			"Width = (1..50);"
			"BatchSize = (1..5);"
			"VectorSize = (1..20);"
			"Values = (-1..1);"
			"Channels = (1..5);"
			"TestCount = 100;"
		),
		CTestParams(
			"Height = (100..500);"
			"Width = (100..500);"
			"BatchSize = (1..5);"
			"VectorSize = (30..50);"
			"Values = (-1..1);"
			"Channels = (1..5);"
			"TestCount = 5;"
		)
	)
);

TEST_P( CRowMultiplyMatrixByMatrixTest, Random )
{
	const auto met = MathEngine().GetType();
	if(met != MET_Cpu && met != MET_Cuda) {
		NEOML_HILIGHT( GTEST_LOG_( INFO ) ) << "Skip for MathEngine type= " << met << " , investigate later.\n";
		return;
	}

	RUN_TEST_IMPL( rowMultiplyMatrixByMatrixTestImpl )
}
