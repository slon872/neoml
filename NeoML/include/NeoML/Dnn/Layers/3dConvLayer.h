/* Copyright © 2017-2020 ABBYY Production LLC

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

#pragma once

#include <NeoML/NeoMLDefs.h>
#include <NeoML/Dnn/Layers/BatchNormalizationLayer.h>
#include <NeoML/Dnn/Dnn.h>
#include <NeoML/Dnn/Layers/ConvLayer.h>

namespace NeoML {

// CBase3dConvLayer is the base class for layers performing 3d-convolution
class NEOML_API CBase3dConvLayer : public CBaseConvLayer {
public:
	// Filter size
	int GetFilterDepth() const { return filterDepth; }
	void SetFilterDepth(int filterDepth);

	// Filter stride
	int GetStrideDepth() const { return strideDepth; }
	void SetStrideDepth(int _strideDepth);

	// Image padding (zero-filled sheets added on to the image edges)
	// It is the same for the opposite sides
	int GetPaddingDepth() const { return paddingDepth; }
	void SetPaddingDepth(int _paddingDepth);

	void Serialize( CArchive& archive ) override;

protected:
	CBase3dConvLayer( IMathEngine& mathEngine, const char* name );

	int filterDepth;			// filter size: depth
	int strideDepth;			// filter stride: depth
	int paddingDepth;			// image padding along the depth dimension
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

// C3dConvLayer implements a 3d-convolution layer
class NEOML_API C3dConvLayer : public CBase3dConvLayer {
	NEOML_DNN_LAYER( C3dConvLayer )
public:
	explicit C3dConvLayer( IMathEngine& mathEngine );

	void Serialize( CArchive& archive ) override;

protected:
	~C3dConvLayer() override;

	void Reshape() override;
	void RunOnce() override;
	void BackwardOnce() override;
	void LearnOnce() override;
	int BlobsForBackward() const override { return 0; }
	int BlobsForLearn() const override { return TInputBlobs; }

private:
	void calcOutputBlobSize(int& outputHeight, int& outputWidth, int& outputDepth) const;

	C3dConvolutionDesc* convDesc;
	void initConvDesc();
	void destroyConvDesc();
};

// N.B. Layer does not support dilation! 
// The Dilation parameter in the CConvAxisParams will be ignored.
NEOML_API CLayerWrapper<C3dConvLayer> Conv3d( int filterCount,
	const CConvAxisParams& heightParams,
	const CConvAxisParams& widthParams,
	const CConvAxisParams& depthParams,
	bool isZeroFreeTerm = false );

} // namespace NeoML
