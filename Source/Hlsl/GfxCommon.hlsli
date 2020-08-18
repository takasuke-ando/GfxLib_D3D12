#ifndef __INCLUDE_GFXCOMMON_HLSLI__
#define __INCLUDE_GFXCOMMON_HLSLI__




float   Noise(float2 src)
{
    // ìKìñÇ»ÉmÉCÉYä÷êî

    float x = dot(src, float2(26.439f, 48.236f));

    x = frac(sin(x) * 347.975);
    //x = frac(x);
    //x = -2 * x * x * x + 3 * x * x;

    return frac(x * 578.229f);


}





#endif // !__INCLUDE_GFXCOMMON_HLSLI__
