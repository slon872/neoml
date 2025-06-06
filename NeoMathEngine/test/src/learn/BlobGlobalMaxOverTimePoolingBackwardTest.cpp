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

static void globalMaxOverTimePoolingBackwardNaive( const float* resultDiff, int batchWidth, int objectSize, float* sourceDiff, int* maxIndices )
{
	for( int j = 0; j < objectSize * batchWidth; ++j ) {
		const int maxIndex = maxIndices[j];
		sourceDiff[maxIndex * objectSize * batchWidth + j] += resultDiff[j];
	}
}

static void globalMaxOverTimePoolingTestImpl( const CTestParams& params, int seed )
{
	CRandom random( seed );

	const CInterval batchLengthInterval = params.GetInterval( "BatchLength" );
	const CInterval batchWidthInterval = params.GetInterval( "BatchWidth" );
	const CInterval objectSizeInterval = params.GetInterval( "ObjectSize" );
	const CInterval valuesInterval = params.GetInterval( "Values" );

	const int batchLength = random.UniformInt( batchLengthInterval.Begin, batchLengthInterval.End );
	const int batchWidth = random.UniformInt( batchWidthInterval.Begin, batchWidthInterval.End );
	const int objectSize = random.UniformInt( objectSizeInterval.Begin, objectSizeInterval.End );

	CREATE_FILL_FLOAT_ARRAY( sourceData, valuesInterval.Begin, valuesInterval.End, objectSize * batchWidth * batchLength, random )
	CFloatBlob sourceBlob( MathEngine(), batchLength, batchWidth, 1, 1, 1, 1, objectSize );
	sourceBlob.CopyFrom( sourceData.data() );
	CFloatBlob sourceDiffBlob( MathEngine(), batchLength, batchWidth, 1, 1, 1, 1, objectSize );

	CREATE_FILL_FLOAT_ARRAY( resultDiffData, valuesInterval.Begin, valuesInterval.End, objectSize * batchWidth, random )
	CFloatBlob resultDiffBlob( MathEngine(), 1, batchWidth, 1, 1, 1, 1, objectSize );
	resultDiffBlob.CopyFrom( resultDiffData.data() );
	CFloatBlob resultBlob( MathEngine(), 1, batchWidth, 1, 1, 1, 1, objectSize );

	CIntBlob indexBlob( MathEngine(), 1, batchWidth, 1, 1, 1, 1, objectSize );
	CIntHandle indexBlobPtr = indexBlob.GetData();

	CGlobalMaxOverTimePoolingDesc* desc = MathEngine().InitGlobalMaxOverTimePooling( sourceBlob.GetDesc(), resultBlob.GetDesc() );
	MathEngine().BlobGlobalMaxOverTimePooling( *desc, sourceBlob.GetData(), &indexBlobPtr, resultBlob.GetData() );
	MathEngine().BlobGlobalMaxOverTimePoolingBackward( *desc, resultDiffBlob.GetData(), indexBlobPtr, sourceDiffBlob.GetData() );
	delete desc;

	std::vector<int> maxIndices;
	maxIndices.resize( objectSize * batchWidth );
	indexBlob.CopyTo( maxIndices.data() );

	std::vector<float> expected, actual;
	actual.resize( batchLength * objectSize * batchWidth );
	sourceDiffBlob.CopyTo( actual.data() );
	expected.insert( expected.begin(), batchLength * objectSize * batchWidth, 0 );
	globalMaxOverTimePoolingBackwardNaive( resultDiffData.data(), batchWidth, objectSize, expected.data(), maxIndices.data() );

	for( int i = 0; i < objectSize * batchWidth; i++ ) {
		EXPECT_NEAR( expected[i], actual[i], 1e-3 );
	}
}

//------------------------------------------------------------------------------------------------------------

class CMathEngineBlobGlobalMaxOverTimePoolingBackwardTest : public CTestFixtureWithParams {
};

INSTANTIATE_TEST_CASE_P( CMathEngineBlobGlobalMaxOverTimePoolingBackwardTestInstantiation, CMathEngineBlobGlobalMaxOverTimePoolingBackwardTest,
	::testing::Values(
		CTestParams(
			"BatchLength = (10..30);"
			"BatchWidth = (5..50);"
			"ObjectSize = (1..100);"
			"Values = (-50..50);"
			"TestCount = 100;"
		)
	)
);

TEST_P( CMathEngineBlobGlobalMaxOverTimePoolingBackwardTest, Random )
{
	const auto met = MathEngine().GetType();
	if(met != MET_Cpu && met != MET_Cuda) {
		NEOML_HILIGHT( GTEST_LOG_( INFO ) ) << "Skipped rest of test for MathEngine type=" << met << " because no implementation.\n";
		return;
	}

	RUN_TEST_IMPL( globalMaxOverTimePoolingTestImpl );
}
