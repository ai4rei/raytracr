OUT_FILES = \
    google\raytracr.h \
    google\raytracr\camera.inc \
    google\raytracr\color.inc \
    google\raytracr\hit3d.inc \
    google\raytracr\light.inc \
    google\raytracr\matrix.inc \
    google\raytracr\matrix3d.inc \
    google\raytracr\object.inc \
    google\raytracr\plane.inc \
    google\raytracr\plane3d.inc \
    google\raytracr\ray3d.inc \
    google\raytracr\sphere.inc \
    google\raytracr\sphere3d.inc \
    google\raytracr\triangle.inc \
    google\raytracr\vector3d.inc

all : googlify

googlify : init $(OUT_FILES) bakclean test

init :
!IF !EXIST(google)
    mkdir google
!ENDIF
!IF !EXIST(google\raytracr)
    mkdir google\raytracr
!ENDIF

test :
    $(CC) -nologo -W4 -wd4512 -Zs -EHsc -TP google\raytracr.h

google\raytracr.h : raytracr.hpp
    copy $? $@ > nul
## class member variables
    quickxlt -wordonly "m_nImageW" "image_width_" $@
    quickxlt -wordonly "m_nImageH" "image_height_" $@
    quickxlt -wordonly "m_uBounceDepth" "bound_depth_" $@
    quickxlt -wordonly "m_uAntiAliasing" "anti_aliasing_" $@
    quickxlt -wordonly "m_ovecEye" "eye_" $@
    quickxlt -wordonly "m_rvecLookAt" "look_at_" $@
    quickxlt -wordonly "m_rvecUp" "up_" $@
    quickxlt -wordonly "m_aclrPixels" "pixels_" $@
    quickxlt -wordonly "m_aobjObjects" "objects_" $@
    quickxlt -wordonly "m_aobjLights" "lights_" $@
    quickxlt -wordonly "m_lpfnProgress" "progress_func_" $@
## arguments and local variables
    quickxlt -wordonly "nX" "x" $@
    quickxlt -wordonly "nY" "y" $@
    quickxlt -wordonly "nZ" "z" $@
    quickxlt -wordonly "nR" "r" $@
    quickxlt -wordonly "nG" "g" $@
    quickxlt -wordonly "nB" "b" $@
    quickxlt -wordonly "lpContext" "context" $@
    quickxlt -wordonly "objObject" "object" $@
    quickxlt -wordonly "objLight" "light" $@
    quickxlt -wordonly "nImageW" "image_width" $@
    quickxlt -wordonly "nImageH" "image_height" $@
    quickxlt -wordonly "uBounceDepth" "bounce_depth" $@
    quickxlt -wordonly "uLevel" "level" $@
    quickxlt -wordonly "ovecEye" "eye" $@
    quickxlt -wordonly "rvecLookAt" "look_at" $@
    quickxlt -wordonly "rvecUp" "up" $@
    quickxlt -wordonly "lpfnProgress" "progress_func" $@
    quickxlt -wordonly "Ray" "ray" $@
    quickxlt -wordonly "nDistanceMin" "distance_min" $@
    quickxlt -wordonly "nDistanceMax" "distance_max" $@
    quickxlt -wordonly "bSingleHit" "single_hit" $@
    quickxlt -wordonly "uDepth" "depth" $@
    quickxlt -wordonly "ResultHit" "result_hit" $@
    quickxlt -wordonly "uIdx" "idx" $@
    quickxlt -wordonly "Hit" "hit" $@
    quickxlt -wordonly "clrLight" "light_color" $@
    quickxlt -wordonly "ovecHitPoint" "hit_point" $@
    quickxlt -wordonly "rvecDirLight" "light_direction" $@
    quickxlt -wordonly "ShadowRay" "shadow_ray" $@
    quickxlt -wordonly "Light" "light" $@
    quickxlt -wordonly "ShadowHit" "shadow_hit" $@
    quickxlt -wordonly "SecondaryHit" "secondary_hit" $@
    quickxlt -wordonly "Cam" "cam" $@
    quickxlt -wordonly "nImageX" "image_x" $@
    quickxlt -wordonly "nImageY" "image_y" $@
    quickxlt -wordonly "clrHit" "hit_color" $@
    quickxlt -wordonly "nPoints" "points" $@
    quickxlt -wordonly "nInc" "increment" $@
    quickxlt -wordonly "nT" "t" $@
    quickxlt -wordonly "nSubX" "sub_x" $@
    quickxlt -wordonly "nSubY" "sub_y" $@
    quickxlt -wordonly "clrPixel" "pixel_color" $@
    quickxlt -wordonly "ovecA" "a" $@
    quickxlt -wordonly "ovecB" "b" $@
    quickxlt -wordonly "ovecC" "c" $@
    quickxlt -wordonly "clrColor" "color" $@
    quickxlt -wordonly "ovecOrigin" "origin" $@
    quickxlt -wordonly "rvecNormal" "normal" $@
    quickxlt -wordonly "nRadius" "radius" $@
    quickxlt -wordonly "nRange" "range" $@
    quickxlt -wordonly "nAtt0" "attenuation0" $@
    quickxlt -wordonly "nAtt1" "attenuation1" $@
    quickxlt -wordonly "nAtt2" "attenuation2" $@
## type names
    quickxlt -wordonly "OBJECTVECTOR" "ObjectVector" $@
    quickxlt -wordonly "LIGHTVECTOR" "LightVector" $@
    quickxlt -wordonly "PIXELVECTOR" "PixelVector" $@
    quickxlt -wordonly "PROGRESSCB" "ProgressCallback" $@
    quickxlt -wordonly "CCamera::TYPE_PERSPECTIVE" "CCamera::kTypePerspective" $@
## classes
    quickxlt -wordonly "CCamera" "Camera" $@
    quickxlt -wordonly "CCameraType" "CameraType" $@
    quickxlt -wordonly "CObject" "Object" $@
    quickxlt -wordonly "CLight" "Light" $@
    quickxlt -wordonly "CColor" "Color" $@
    quickxlt -wordonly "CHit3d" "Hit3d" $@
    quickxlt -wordonly "CRay3d" "Ray3d" $@
    quickxlt -wordonly "CVector3d" "Vector3d" $@
    quickxlt -wordonly "CPlane3d" "Plane3d" $@
    quickxlt -wordonly "CTriangle" "Triangle" $@
    quickxlt -wordonly "CPlane" "Plane" $@
    quickxlt -wordonly "CSphere" "Sphere" $@
    quickxlt -wordonly "CMatrix3d" "Matrix3d" $@
## misc
    quickxlt -wordonly SNIPPETS_RAYTRACER_HPP RAYTRACR_HPP_ $@
    quickxlt "#include \x22_raytracr/" "#include \x22raytracr/" $@
    quickxlt ".hpp\x22\x0D\x0A" ".inc\x22\x0D\x0A" $@
    quickxlt "    " "\t" $@
    quickxlt "\r\n{" " {" $@
    quickxlt "\r\n\t{" " {" $@
    quickxlt "\r\n\t\t{" " {" $@
    quickxlt "\r\n\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t\t{" " {" $@
    quickxlt "\t:" "    :" $@
    quickxlt "\t," "    ," $@
    quickxlt "\t" "  " $@

google\raytracr\camera.inc : _raytracr\camera.hpp
    copy $? $@ > nul
## class member variables
    quickxlt -wordonly "m_ovecEye" "eye_" $@
    quickxlt -wordonly "m_rvecLookAt" "look_at_" $@
    quickxlt -wordonly "m_uvecRight" "right_" $@
    quickxlt -wordonly "m_uvecUp" "up_" $@
    quickxlt -wordonly "m_nFovH" "fov_height_" $@
    quickxlt -wordonly "m_nFovW" "fov_width_" $@
    quickxlt -wordonly "m_nFovXUnit" "fov_x_unit_" $@
    quickxlt -wordonly "m_nFovYUnit" "fov_y_unit_" $@
    quickxlt -wordonly "m_rvecFOV0" "fov0_" $@
    quickxlt -wordonly "m_rvecLookAt0" "look_at0_" $@
    quickxlt -wordonly "m_nType" "type_" $@
## arguments and local variables
    quickxlt -wordonly "ovecEye" "eye" $@
    quickxlt -wordonly "rvecLookAt" "look_at" $@
    quickxlt -wordonly "rvecUp" "up" $@
    quickxlt -wordonly "nType" "type" $@
    quickxlt -wordonly "nFOV" "fov" $@
    quickxlt -wordonly "nImageW" "image_width" $@
    quickxlt -wordonly "nImageH" "image_height" $@
    quickxlt -wordonly "nImageX" "image_x" $@
    quickxlt -wordonly "nImageY" "image_y" $@
    quickxlt -wordonly "nSubX" "sub_x" $@
    quickxlt -wordonly "nSubY" "sub_y" $@
    quickxlt -wordonly "rvecX" "x" $@
    quickxlt -wordonly "rvecY" "y" $@
    quickxlt -wordonly "rvecXY" "xy" $@
    quickxlt -wordonly "rvecRay" "ray" $@
    quickxlt -wordonly "uvecRay" "unit_ray" $@
## type names
    quickxlt -wordonly "TYPE_PERSPECTIVE" "kTypePerspective" $@
    quickxlt -wordonly "TYPE" "Type" $@
## classes
    quickxlt -wordonly "CCamera" "Camera" $@
    quickxlt -wordonly "CRay3d" "Ray3d" $@
    quickxlt -wordonly "CVector3d" "Vector3d" $@
    quickxlt -wordonly "CMatrix3d" "Matrix3d" $@
## misc
    quickxlt -wordonly SNIPPETS_RAYTRACER_CAMERA_HPP RAYTRACR_CAMERA_INC_ $@
    quickxlt "    " "\t" $@
    quickxlt "\r\n{" " {" $@
    quickxlt "\r\n\t{" " {" $@
    quickxlt "\r\n\t\t{" " {" $@
    quickxlt "\r\n\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t\t{" " {" $@
    quickxlt "\t:" "    :" $@
    quickxlt "\t," "    ," $@
    quickxlt "\t" "  " $@

google\raytracr\color.inc : _raytracr\color.hpp
    copy $? $@ > nul
## class member variables
    quickxlt -wordonly "m_nR" "r_" $@
    quickxlt -wordonly "m_nG" "g_" $@
    quickxlt -wordonly "m_nB" "b_" $@
## arguments and local variables
    quickxlt -wordonly "nR" "r" $@
    quickxlt -wordonly "nG" "g" $@
    quickxlt -wordonly "nB" "b" $@
    quickxlt -wordonly "clrOther" "other" $@
    quickxlt -wordonly "nScale" "scale" $@
## type names
## classes
    quickxlt -wordonly "CColor" "Color" $@
## misc
    quickxlt -wordonly SNIPPETS_RAYTRACER_COLOR_HPP RAYTRACR_COLOR_INC_ $@
    quickxlt "    " "\t" $@
    quickxlt "\r\n{" " {" $@
    quickxlt "\r\n\t{" " {" $@
    quickxlt "\r\n\t\t{" " {" $@
    quickxlt "\r\n\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t\t{" " {" $@
    quickxlt "\t:" "    :" $@
    quickxlt "\t," "    ," $@
    quickxlt "\t" "  " $@

google\raytracr\hit3d.inc : _raytracr\hit3d.hpp
    copy $? $@ > nul
## class member variables
    quickxlt -wordonly "m_nDistance" "distance_" $@
    quickxlt -wordonly "m_clrColor" "color_" $@
    quickxlt -wordonly "m_uvecSurfaceNormal" "surface_normal_" $@
    quickxlt -wordonly "m_bHit" "hit_" $@
## arguments and local variables
    quickxlt -wordonly "nDistance" "distance" $@
    quickxlt -wordonly "clrColor" "color" $@
    quickxlt -wordonly "uvecSurfaceNormal" "surface_normal" $@
## type names
## classes
    quickxlt -wordonly "CHit3d" "Hit3d" $@
    quickxlt -wordonly "CColor" "Color" $@
    quickxlt -wordonly "CVector3d" "Vector3d" $@
## misc
    quickxlt -wordonly SNIPPETS_RAYTRACER_HIT3D_HPP RAYTRACR_HIT3D_INC_ $@
    quickxlt "    " "\t" $@
    quickxlt "\r\n{" " {" $@
    quickxlt "\r\n\t{" " {" $@
    quickxlt "\r\n\t\t{" " {" $@
    quickxlt "\r\n\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t\t{" " {" $@
    quickxlt "\t:" "    :" $@
    quickxlt "\t," "    ," $@
    quickxlt "\t" "  " $@

google\raytracr\light.inc : _raytracr\light.hpp
    copy $? $@ > nul
## class member variables
    quickxlt -wordonly "m_ovecOrigin" "origin_" $@
    quickxlt -wordonly "m_clrColor" "color_" $@
    quickxlt -wordonly "m_nAtt0" "attenuation0_" $@
    quickxlt -wordonly "m_nAtt1" "attenuation1_" $@
    quickxlt -wordonly "m_nAtt2" "attenuation2_" $@
    quickxlt -wordonly "m_nRange" "range_" $@
## arguments and local variables
    quickxlt -wordonly "ovecOrigin" "origin" $@
    quickxlt -wordonly "clrColor" "color" $@
    quickxlt -wordonly "nRange" "range" $@
    quickxlt -wordonly "nAtt0" "attenuation0" $@
    quickxlt -wordonly "nAtt1" "attenuation1" $@
    quickxlt -wordonly "nAtt2" "attenuation2" $@
    quickxlt -wordonly "nAtt" "attenuation" $@
    quickxlt -wordonly "nDistance" "distance" $@
## type names
## classes
    quickxlt -wordonly "CLight" "Light" $@
    quickxlt -wordonly "CColor" "Color" $@
    quickxlt -wordonly "CVector3d" "Vector3d" $@
## misc
    quickxlt -wordonly SNIPPETS_RAYTRACER_LIGHT_HPP RAYTRACR_LIGHT_INC_ $@
    quickxlt "    " "\t" $@
    quickxlt "\r\n{" " {" $@
    quickxlt "\r\n\t{" " {" $@
    quickxlt "\r\n\t\t{" " {" $@
    quickxlt "\r\n\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t\t{" " {" $@
    quickxlt "\t:" "    :" $@
    quickxlt "\t," "    ," $@
    quickxlt "\t" "  " $@

google\raytracr\matrix.inc : _raytracr\matrix.hpp
    copy $? $@ > nul
## class member variables
    quickxlt -wordonly "m_aItem" "item_" $@
## arguments and local variables
    quickxlt -wordonly "I" "i" $@
    quickxlt -wordonly "J" "j" $@
    quickxlt -wordonly "nScalar" "scalar" $@
    quickxlt -wordonly "nValue" "value" $@
    quickxlt -wordonly "mtxOther" "other" $@
    quickxlt -wordonly "mtxResult" "result" $@
## type names
## classes
    quickxlt -wordonly "CMatrix" "Matrix" $@
    quickxlt -wordonly "CSquareMatrix" "SquareMatrix" $@
    quickxlt -wordonly "CBaseMatrix" "BaseMatrix" $@
    quickxlt -wordonly "CBaseSquareMatrix" "BaseSquareMatrix" $@
    quickxlt -wordonly "CSelf" "Self" $@
## misc
    quickxlt -wordonly SNIPPETS_RAYTRACER_MATRIX_HPP RAYTRACR_MATRIX_INC_ $@
    quickxlt "    " "\t" $@
    quickxlt "\r\n{" " {" $@
    quickxlt "\r\n\t{" " {" $@
    quickxlt "\r\n\t\t{" " {" $@
    quickxlt "\r\n\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t\t{" " {" $@
    quickxlt "\t:" "    :" $@
    quickxlt "\t," "    ," $@
    quickxlt "\t" "  " $@

google\raytracr\matrix3d.inc : _raytracr\matrix3d.hpp
    copy $? $@ > nul
## class member variables
## arguments and local variables
    quickxlt -wordonly "nX1" "x1" $@
    quickxlt -wordonly "nX2" "x2" $@
    quickxlt -wordonly "nX3" "x3" $@
    quickxlt -wordonly "nY1" "y1" $@
    quickxlt -wordonly "nY2" "y2" $@
    quickxlt -wordonly "nY3" "y3" $@
    quickxlt -wordonly "nZ1" "z1" $@
    quickxlt -wordonly "nZ2" "z2" $@
    quickxlt -wordonly "nZ3" "z3" $@
    quickxlt -wordonly "uvecUp" "up" $@
    quickxlt -wordonly "uvecRight" "right" $@
    quickxlt -wordonly "uvecBack" "back" $@
    quickxlt -wordonly "nOffset" "offset" $@
    quickxlt -wordonly "nRad" "rad" $@
    quickxlt -wordonly "mtxOffset" "offset_matrix" $@
    quickxlt -wordonly "mtxRoll" "roll_matrix" $@
    quickxlt -wordonly "mtxPitch" "pitch_matrix" $@
    quickxlt -wordonly "mtxYaw" "yaw_matrix" $@
## type names
    quickxlt -wordonly "ROW_X" "kRowX" $@
    quickxlt -wordonly "ROW_Y" "kRowY" $@
    quickxlt -wordonly "ROW_Z" "kRowZ" $@
    quickxlt -wordonly "ROW_W" "kRowW" $@
## classes
    quickxlt -wordonly "CMatrix3d" "Matrix3d" $@
    quickxlt -wordonly "CBaseSquareMatrix" "BaseSquareMatrix" $@
    quickxlt -wordonly "CVector3d" "Vector3d" $@
    quickxlt -wordonly "CSelf" "Self" $@
## misc
    quickxlt -wordonly SNIPPETS_RAYTRACER_MATRIX3D_HPP RAYTRACR_MATRIX3D_INC_ $@
    quickxlt "    " "\t" $@
    quickxlt "\r\n{" " {" $@
    quickxlt "\r\n\t{" " {" $@
    quickxlt "\r\n\t\t{" " {" $@
    quickxlt "\r\n\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t\t{" " {" $@
    quickxlt "\t:" "    :" $@
    quickxlt "\t," "    ," $@
    quickxlt "\t" "  " $@

google\raytracr\object.inc : _raytracr\object.hpp
    copy $? $@ > nul
## class member variables
    quickxlt -wordonly "m_clrColor" "color_" $@
## arguments and local variables
    quickxlt -wordonly "clrColor" "color" $@
    quickxlt -wordonly "Ray" "ray" $@
    quickxlt -wordonly "nDistanceMin" "distance_min" $@
    quickxlt -wordonly "nDistanceMax" "distance_max" $@
## type names
## classes
    quickxlt -wordonly "CObject" "Object" $@
    quickxlt -wordonly "CColor" "Color" $@
    quickxlt -wordonly "CHit3d" "Hit3d" $@
    quickxlt -wordonly "CRay3d" "Ray3d" $@
    quickxlt -wordonly "CVector3d" "Vector3d" $@
## misc
    quickxlt -wordonly SNIPPETS_RAYTRACER_OBJECT_HPP RAYTRACR_OBJECT_INC_ $@
    quickxlt "    " "\t" $@
    quickxlt "\r\n{" " {" $@
    quickxlt "\r\n\t{" " {" $@
    quickxlt "\r\n\t\t{" " {" $@
    quickxlt "\r\n\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t\t{" " {" $@
    quickxlt "\t:" "    :" $@
    quickxlt "\t," "    ," $@
    quickxlt "\t" "  " $@

google\raytracr\plane.inc : _raytracr\plane.hpp
    copy $? $@ > nul
## class member variables
    quickxlt -wordonly "m_ovecOrigin" "origin_" $@
    quickxlt -wordonly "m_uvecNormal" "normal_" $@
    quickxlt -wordonly "m_clrColor" "color_" $@
## arguments and local variables
    quickxlt -wordonly "ovecOrigin" "origin" $@
    quickxlt -wordonly "rvecNormal" "normal" $@
    quickxlt -wordonly "clrColor" "color" $@
    quickxlt -wordonly "Ray" "ray" $@
    quickxlt -wordonly "nDistanceMin" "distance_min" $@
    quickxlt -wordonly "nDistanceMax" "distance_max" $@
    quickxlt -wordonly "nDenominator" "denominator" $@
    quickxlt -wordonly "nCounter" "counter" $@
    quickxlt -wordonly "nDistance" "distance" $@
## type names
## classes
    quickxlt -wordonly "CPlane" "Plane" $@
    quickxlt -wordonly "CObject" "Object" $@
    quickxlt -wordonly "CColor" "Color" $@
    quickxlt -wordonly "CHit3d" "Hit3d" $@
    quickxlt -wordonly "CRay3d" "Ray3d" $@
    quickxlt -wordonly "CVector3d" "Vector3d" $@
## misc
    quickxlt -wordonly SNIPPETS_RAYTRACER_PLANE_HPP RAYTRACR_PLANE_INC_ $@
    quickxlt "    " "\t" $@
    quickxlt "\r\n{" " {" $@
    quickxlt "\r\n\t{" " {" $@
    quickxlt "\r\n\t\t{" " {" $@
    quickxlt "\r\n\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t\t{" " {" $@
    quickxlt "\t:" "    :" $@
    quickxlt "\t," "    ," $@
    quickxlt "\t" "  " $@

google\raytracr\plane3d.inc : _raytracr\plane3d.hpp
    copy $? $@ > nul
## class member variables
    quickxlt -wordonly "m_ovecOrigin" "origin_" $@
    quickxlt -wordonly "m_uvecNormal" "normal_" $@
## arguments and local variables
    quickxlt -wordonly "ovecOrigin" "origin" $@
    quickxlt -wordonly "uvecNormal" "normal" $@
    quickxlt -wordonly "ovecPoint" "point" $@
    quickxlt -wordonly "vecVector" "vector" $@
    quickxlt -wordonly "plnOther" "other" $@
    quickxlt -wordonly "rvecDirection" "direction" $@
    quickxlt -wordonly "rvecOL" "ol" $@
    quickxlt -wordonly "rvecON" "on" $@
    quickxlt -wordonly "rvecLN" "ln" $@
    quickxlt -wordonly "rvecNR" "nr" $@
    quickxlt -wordonly "rvecOR" "or" $@
## type names
## classes
    quickxlt -wordonly "CPlane3d" "Plane3d" $@
    quickxlt -wordonly "CVector3d" "Vector3d" $@
    quickxlt -wordonly "CSelf" "Self" $@
## misc
    quickxlt -wordonly SNIPPETS_RAYTRACER_PLANE3D_HPP RAYTRACR_PLANE3D_INC_ $@
    quickxlt "    " "\t" $@
    quickxlt "\r\n{" " {" $@
    quickxlt "\r\n\t{" " {" $@
    quickxlt "\r\n\t\t{" " {" $@
    quickxlt "\r\n\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t\t{" " {" $@
    quickxlt "\t:" "    :" $@
    quickxlt "\t," "    ," $@
    quickxlt "\t" "  " $@

google\raytracr\ray3d.inc : _raytracr\ray3d.hpp
    copy $? $@ > nul
## class member variables
    quickxlt -wordonly "m_ovecEye" "eye_" $@
    quickxlt -wordonly "m_uvecLookAt" "look_at_" $@
## arguments and local variables
    quickxlt -wordonly "ovecEye" "eye" $@
    quickxlt -wordonly "uvecLookAt" "look_at" $@
    quickxlt -wordonly "nDistance" "distance" $@
## type names
## classes
    quickxlt -wordonly "CRay3d" "Ray3d" $@
    quickxlt -wordonly "CVector3d" "Vector3d" $@
## misc
    quickxlt -wordonly SNIPPETS_RAYTRACER_RAY3D_HPP RAYTRACR_RAY3D_INC_ $@
    quickxlt "    " "\t" $@
    quickxlt "\r\n{" " {" $@
    quickxlt "\r\n\t{" " {" $@
    quickxlt "\r\n\t\t{" " {" $@
    quickxlt "\r\n\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t\t{" " {" $@
    quickxlt "\t:" "    :" $@
    quickxlt "\t," "    ," $@
    quickxlt "\t" "  " $@

google\raytracr\sphere.inc : _raytracr\sphere.hpp
    copy $? $@ > nul
## class member variables
    quickxlt -wordonly "m_ovecOrigin" "origin_" $@
    quickxlt -wordonly "m_nRadius" "radius_" $@
    quickxlt -wordonly "m_nRadius2" "radius2_" $@
    quickxlt -wordonly "m_clrColor" "color_" $@
## arguments and local variables
    quickxlt -wordonly "ovecOrigin" "origin" $@
    quickxlt -wordonly "nRadius" "radius" $@
    quickxlt -wordonly "Ray" "ray" $@
    quickxlt -wordonly "nDistanceMin" "distance_min" $@
    quickxlt -wordonly "nDistanceMax" "distance_max" $@
    quickxlt -wordonly "nDenominator" "denominator" $@
    quickxlt -wordonly "nCounter" "counter" $@
    quickxlt -wordonly "nDistance1" "distance1" $@
    quickxlt -wordonly "nDistance2" "distance2" $@
    quickxlt -wordonly "nDistance" "distance" $@
    quickxlt -wordonly "nVcbs" "vcbs" $@
    quickxlt -wordonly "nVusr" "vusr" $@
    quickxlt -wordonly "nVbsr" "vbsr" $@
    quickxlt -wordonly "nVosr" "vosr" $@
## type names
## classes
    quickxlt -wordonly "CSphere" "Sphere" $@
    quickxlt -wordonly "CObject" "Object" $@
    quickxlt -wordonly "CColor" "Color" $@
    quickxlt -wordonly "CHit3d" "Hit3d" $@
    quickxlt -wordonly "CRay3d" "Ray3d" $@
    quickxlt -wordonly "CVector3d" "Vector3d" $@
## misc
    quickxlt -wordonly SNIPPETS_RAYTRACER_SPHERE_HPP RAYTRACR_SPHERE_INC_ $@
    quickxlt "    " "\t" $@
    quickxlt "\r\n{" " {" $@
    quickxlt "\r\n\t{" " {" $@
    quickxlt "\r\n\t\t{" " {" $@
    quickxlt "\r\n\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t\t{" " {" $@
    quickxlt "\t:" "    :" $@
    quickxlt "\t," "    ," $@
    quickxlt "\t" "  " $@

google\raytracr\sphere3d.inc : _raytracr\sphere3d.hpp
    copy $? $@ > nul
## class member variables
    quickxlt -wordonly "m_ovecOrigin" "origin_" $@
    quickxlt -wordonly "m_nRadius" "radius_" $@
    quickxlt -wordonly "m_nRadius2" "radius2_" $@
## arguments and local variables
    quickxlt -wordonly "ovecPointOnSurface" "point_on_surface" $@
    quickxlt -wordonly "rvecDirection" "direction" $@
    quickxlt -wordonly "ovecOrigin" "origin" $@
    quickxlt -wordonly "nRadius" "radius" $@
## type names
## classes
    quickxlt -wordonly "CSphere3d" "Sphere3d" $@
    quickxlt -wordonly "CPlane3d" "Plane3d" $@
    quickxlt -wordonly "CVector3d" "Vector3d" $@
    quickxlt -wordonly "CSelf" "Self" $@
## misc
    quickxlt -wordonly SNIPPETS_RAYTRACER_SPHERE3D_HPP RAYTRACR_SPHERE3D_INC_ $@
    quickxlt "    " "\t" $@
    quickxlt "\r\n{" " {" $@
    quickxlt "\r\n\t{" " {" $@
    quickxlt "\r\n\t\t{" " {" $@
    quickxlt "\r\n\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t\t{" " {" $@
    quickxlt "\t:" "    :" $@
    quickxlt "\t," "    ," $@
    quickxlt "\t" "  " $@

google\raytracr\triangle.inc : _raytracr\triangle.hpp
    copy $? $@ > nul
## class member variables
    quickxlt -wordonly "m_ovecA" "a_" $@
    quickxlt -wordonly "m_ovecB" "b_" $@
    quickxlt -wordonly "m_ovecC" "c_" $@
    quickxlt -wordonly "m_uvecNormal" "normal_" $@
    quickxlt -wordonly "m_clrColor" "color_" $@
## arguments and local variables
    quickxlt -wordonly "ovecA" "a" $@
    quickxlt -wordonly "ovecB" "b" $@
    quickxlt -wordonly "ovecC" "c" $@
    quickxlt -wordonly "ovecP" "p" $@
    quickxlt -wordonly "clrColor" "color" $@
    quickxlt -wordonly "Ray" "ray" $@
    quickxlt -wordonly "nDistanceMin" "distance_min" $@
    quickxlt -wordonly "nDistanceMax" "distance_max" $@
    quickxlt -wordonly "nDenominator" "denominator" $@
    quickxlt -wordonly "nCounter" "counter" $@
    quickxlt -wordonly "nDistance" "distance" $@
## type names
## classes
    quickxlt -wordonly "CTriangle" "Triangle" $@
    quickxlt -wordonly "CObject" "Object" $@
    quickxlt -wordonly "CColor" "Color" $@
    quickxlt -wordonly "CHit3d" "Hit3d" $@
    quickxlt -wordonly "CRay3d" "Ray3d" $@
    quickxlt -wordonly "CVector3d" "Vector3d" $@
## misc
    quickxlt -wordonly SNIPPETS_RAYTRACER_TRIANGLE_HPP RAYTRACR_TRIANGLE_INC_ $@
    quickxlt "    " "\t" $@
    quickxlt "\r\n{" " {" $@
    quickxlt "\r\n\t{" " {" $@
    quickxlt "\r\n\t\t{" " {" $@
    quickxlt "\r\n\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t\t{" " {" $@
    quickxlt "\t:" "    :" $@
    quickxlt "\t," "    ," $@
    quickxlt "\t" "  " $@

google\raytracr\vector3d.inc : _raytracr\vector3d.hpp
    copy $? $@ > nul
## class member variables
    quickxlt -wordonly "m_nX" "x_" $@
    quickxlt -wordonly "m_nY" "y_" $@
    quickxlt -wordonly "m_nZ" "z_" $@
    quickxlt -wordonly "m_nMagnitude" "magnitude_" $@
## arguments and local variables
    quickxlt -wordonly "nX" "x" $@
    quickxlt -wordonly "nY" "y" $@
    quickxlt -wordonly "nZ" "z" $@
    quickxlt -wordonly "vecOther" "other" $@
    quickxlt -wordonly "nScale" "scale" $@
    quickxlt -wordonly "ovecPoint" "point" $@
    quickxlt -wordonly "ovecOrigin" "origin" $@
    quickxlt -wordonly "mtxOther" "other" $@
## type names
## classes
    quickxlt -wordonly "CVector3d" "Vector3d" $@
    quickxlt -wordonly "CMatrix3d" "Matrix3d" $@
    quickxlt -wordonly "CSelf" "Self" $@
## misc
    quickxlt -wordonly SNIPPETS_RAYTRACER_VECTOR3D_HPP RAYTRACR_VECTOR3D_INC_ $@
    quickxlt "    " "\t" $@
    quickxlt "\r\n{" " {" $@
    quickxlt "\r\n\t{" " {" $@
    quickxlt "\r\n\t\t{" " {" $@
    quickxlt "\r\n\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t{" " {" $@
    quickxlt "\r\n\t\t\t\t\t{" " {" $@
    quickxlt "\t:" "    :" $@
    quickxlt "\t," "    ," $@
    quickxlt "\t" "  " $@

bakclean :
    if exist google\raytracr\*.bak del google\raytracr\*.bak
    if exist google\*.bak del google\*.bak

clean : bakclean
    if exist google\raytracr\*.inc del google\raytracr\*.inc
    if exist google\raytracr\nul rmdir google\raytracr
    if exist google\raytracr.h del google\raytracr.h
    if exist google\nul rmdir google
