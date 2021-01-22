#pragma once
#include "ResizeableTexture.h"

struct TextureRegion final
{
	ResizeableTexture& texture;
	int from;
	int to;

    static std::vector<TextureRegion> splitTextureIntoRegions(ResizeableTexture& texture, int numberOfRanges)
    {
        const int evenLength = (texture.width + 1) / numberOfRanges;

    	std::vector<int> bucketSizes;

        bucketSizes.reserve(numberOfRanges);
        for (int i = 0; i < numberOfRanges; ++i)
            bucketSizes.push_back(evenLength);

        /* distribute surplus as evenly as possible across buckets */
        int surplus = (texture.width + 1) % numberOfRanges;
    	
        for (int i = 0; surplus > 0; --surplus, i = (i + 1) % numberOfRanges)
            bucketSizes[i] += 1;

        std::vector<TextureRegion> regions;
        int lowerBound = 0;
        for (int i = 0; i < numberOfRanges && lowerBound <= texture.width; ++i)
        {
            regions.push_back({ texture, lowerBound, lowerBound + bucketSizes[i]});
            
            lowerBound += bucketSizes[i];
        }
        return regions;
    }

};
