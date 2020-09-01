#ifndef __INCLUDE_GFXCOMMON_HLSLI__
#define __INCLUDE_GFXCOMMON_HLSLI__




float   Noise(float2 src)
{
    // 適当なノイズ関数

    float x = dot(src, float2(26.439f, 48.236f));

    x = frac(sin(x) * 347.975);
    //x = frac(x);
    //x = -2 * x * x * x + 3 * x * x;

    return frac(x * 578.229f);


}


/*

    2次元上に一様に配置したcount個の座標を取得

*/
float2  Hammersley(uint index, uint count, float2 secondRand)
{


    float x = (index + secondRand.x) / (float)count;

    float y = 0;

    float base = 0.5f;

    uint val = index;
    while (val) {

        if (val & 1) {

            y += base;

        }


        base *= 0.5f;
        val >>= 1;
    }


    y += secondRand.y;

    if (y > 1) y -= 1;


    return float2(x, y);


}



#endif // !__INCLUDE_GFXCOMMON_HLSLI__
