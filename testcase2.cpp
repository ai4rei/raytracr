#include <cmath>
#include <cstdio>
#include <memory>
#include <vector>

template< typename T > const T& min(const T& One, const T& Two)
{
    if(One<Two)
    {
        return One;
    }

    return Two;
}

template< typename T > const T& max(const T& One, const T& Two)
{
    if(One>Two)
    {
        return One;
    }

    return Two;
}

#include "raytracr.hpp"

int __cdecl main(int nArgc, char** lppszArgv)
{
    Raytracer R;

    R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(+0.0f, +0.0f, +0.0f), 0.1f, Raytracer::CreateColor(1.0f, 0.0f, 0.0f)));
    R.AddObject(Raytracer::CreatePlane(Raytracer::CreateVector3d(+0.0f, +0.0f, +0.0f), Raytracer::CreateVector3d(+0.0f, +1.0f, +0.0f), Raytracer::CreateColor(1.0f, 1.0f, 1.0f)));
    R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(+2.0f, +1.0f, -3.0f), 1.0f, Raytracer::CreateColor(0.25f, 0.25f, 0.75f)));
    R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(-2.0f, +1.0f, -3.0f), 1.0f, Raytracer::CreateColor(0.75f, 0.25f, 0.25f)));
    R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(+0.0f, +1.0f, -2.0f), 1.0f, Raytracer::CreateColor(0.25f, 0.75f, 0.25f)));
    R.AddLight(Raytracer::CreateLight(Raytracer::CreateVector3d(+9.0f, +9.0f, +0.0f), Raytracer::CreateColor(1.0f, 1.0f, 1.0f), 50.0f));
    R.SetImageSize(700, 500);
    R.SetCamera(Raytracer::CreateVector3d(+0.0f, +1.0f, +10.0f), Raytracer::CreateVector3d(+0.0f, +0.0f, -1.0f), Raytracer::CreateVector3d(+0.0f, +1.0f, +0.0f));
    R.Render(nullptr);

    const auto aclrPixels = R.GetResult();
    const int nImageW = R.GetImageWidth();
    const int nImageH = R.GetImageHeight();

    printf("P3\n%d %d\n255\n", nImageW, nImageH);

    for(int nImageY = nImageH-1; nImageY>=0; nImageY--)
    {
        for(int nImageX = 0; nImageX<nImageW; nImageX++)
        {
            const auto& Px = aclrPixels[nImageX+nImageY*nImageW];

            printf("%u %u %u\n"
                , static_cast< unsigned char >(Px.R()*255U)
                , static_cast< unsigned char >(Px.G()*255U)
                , static_cast< unsigned char >(Px.B()*255U)
            );
        }
    }

    return 0;
}
