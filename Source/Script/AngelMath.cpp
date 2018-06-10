#include "Core/Core.h"
#include <angelscript.h>
#include <scriptmath\scriptmath.h>
#include "AngelState.h"
#include "../Math/Math.h"
#include "../Math/Vector2f.h"
#include "../Math/Vector2i.h"
#include "../Math/IntRect.h"

using namespace math;

namespace script
{
    int32_t angelMath_random(int32_t low, int32_t high)
    {
        if (low >= high)
        {
            asGetActiveContext()->SetException("Invalid args.");
            return 0;
        }
        return (rand() % (high - low)) + low;
    }

    void angelMath_Vector2f_DefaultConstruct(Vector2f* self)
    {
        new (self) Vector2f();
    }

    void angelMath_Vector2f_Construct1(Vector2f* self, float s)
    {
        new (self) Vector2f(s);
    }

    void angelMath_Vector2f_Construct2(Vector2f* self, float x, float y)
    {
        new (self) Vector2f(x, y);
    }

    void angelMath_Vector2i_DefaultConstruct(Vector2i* self)
    {
        new (self) Vector2i();
    }

    void angelMath_Vector2i_Construct1(Vector2i* self, int32_t s)
    {
        new (self) Vector2i(s);
    }

    void angelMath_Vector2i_Construct2(Vector2i* self, int32_t x, int32_t y)
    {
        new (self) Vector2i(x, y);
    }

    void angelMath_IntRect_DefaultConstruct(IntRect* self)
    {
        new (self) IntRect();
    }

    void angelMath_IntRect_Construct1(IntRect* self, int32_t left, int32_t top, int32_t width, int32_t height)
    {
        new (self) IntRect(left, top, width, height);
    }

    void angelMath_IntRect_Construct2(IntRect* self, Vector2i corner, Vector2i size)
    {
        new (self) IntRect(corner, size);
    }

    void angelMath_RegisterTypes(asIScriptEngine* engine)
    {
        RegisterScriptMath(engine);

        AS_VERIFY(engine->RegisterGlobalFunction("int random(int, int)", asFUNCTION(angelMath_random), asCALL_CDECL));

        AS_VERIFY(engine->RegisterGlobalFunction("float radToDeg(float)", asFUNCTION(math::radToDeg), asCALL_CDECL));
        AS_VERIFY(engine->RegisterGlobalFunction("float degToRad(float)", asFUNCTION(math::degToRad), asCALL_CDECL));

        AS_VERIFY(engine->RegisterGlobalFunction("float lerp(float, float, float)", asFUNCTION(math::lerp), asCALL_CDECL));

        AS_VERIFY(engine->RegisterGlobalFunction("int intSign(int)", asFUNCTIONPR(math::sign, (int), int), asCALL_CDECL));
        AS_VERIFY(engine->RegisterGlobalFunction("int floatSign(float)", asFUNCTIONPR(math::sign, (float), int), asCALL_CDECL));

        AS_VERIFY(engine->RegisterGlobalFunction("int intMin(int, int)", asFUNCTIONPR(math::min, (int, int), int), asCALL_CDECL));
        AS_VERIFY(engine->RegisterGlobalFunction("int intMax(int, int)", asFUNCTIONPR(math::max, (int, int), int), asCALL_CDECL));
        AS_VERIFY(engine->RegisterGlobalFunction("float floatMin(float, float)", asFUNCTIONPR(math::min, (float, float), float), asCALL_CDECL));
        AS_VERIFY(engine->RegisterGlobalFunction("float floatMax(float, float)", asFUNCTIONPR(math::max, (float, float), float), asCALL_CDECL));

        AS_VERIFY(engine->RegisterObjectType("Vector2f", sizeof(Vector2f), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<Vector2f>()));
        AS_VERIFY(engine->RegisterObjectProperty("Vector2f", "float x", asOFFSET(Vector2f, x)));
        AS_VERIFY(engine->RegisterObjectProperty("Vector2f", "float y", asOFFSET(Vector2f, y)));
        AS_VERIFY(engine->RegisterObjectBehaviour("Vector2f", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(angelMath_Vector2f_DefaultConstruct), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectBehaviour("Vector2f", asBEHAVE_CONSTRUCT, "void f(float s)", asFUNCTION(angelMath_Vector2f_Construct1), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectBehaviour("Vector2f", asBEHAVE_CONSTRUCT, "void f(float x, float y)", asFUNCTION(angelMath_Vector2f_Construct2), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("Vector2f", "Vector2f& opNeg()", asMETHODPR(Vector2f, operator-, (void) const, Vector2f), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("Vector2f", "Vector2f& opAddAssign(const Vector2f &in rhs)", asMETHOD(Vector2f, operator+=), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("Vector2f", "Vector2f& opSubAssign(const Vector2f &in rhs)", asMETHOD(Vector2f, operator-=), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("Vector2f", "Vector2f& opMulAssign(int rhs)", asMETHOD(Vector2f, operator*=), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("Vector2f", "Vector2f& opDivAssign(int rhs)", asMETHOD(Vector2f, operator/=), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("Vector2f", "bool opEquals(const Vector2f &in other)", asMETHOD(Vector2f, operator==), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("Vector2f", "Vector2f opAdd(const Vector2f &in other)", asMETHOD(Vector2f, operator+), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("Vector2f", "Vector2f opSub(const Vector2f &in other)", asMETHODPR(Vector2f, operator-, (const Vector2f&) const, Vector2f), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("Vector2f", "Vector2f opMul(int scalar)", asMETHOD(Vector2f, operator*), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("Vector2f", "Vector2f opDiv(int divisor)", asMETHOD(Vector2f, operator/), asCALL_THISCALL));


        AS_VERIFY(engine->RegisterObjectType("Vector2i", sizeof(Vector2i), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<Vector2i>()));
        AS_VERIFY(engine->RegisterObjectProperty("Vector2i", "int x", asOFFSET(Vector2i, x)));
        AS_VERIFY(engine->RegisterObjectProperty("Vector2i", "int y", asOFFSET(Vector2i, y)));
        AS_VERIFY(engine->RegisterObjectBehaviour("Vector2i", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(angelMath_Vector2i_DefaultConstruct), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectBehaviour("Vector2i", asBEHAVE_CONSTRUCT, "void f(int s)", asFUNCTION(angelMath_Vector2i_Construct1), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectBehaviour("Vector2i", asBEHAVE_CONSTRUCT, "void f(int x, int y)", asFUNCTION(angelMath_Vector2i_Construct2), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("Vector2i", "Vector2i& opNeg()", asMETHODPR(Vector2i, operator-, (void) const, Vector2i), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("Vector2i", "Vector2i& opAddAssign(const Vector2i &in rhs)", asMETHOD(Vector2i, operator+=), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("Vector2i", "Vector2i& opSubAssign(const Vector2i &in rhs)", asMETHOD(Vector2i, operator-=), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("Vector2i", "Vector2i& opMulAssign(int rhs)", asMETHOD(Vector2i, operator*=), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("Vector2i", "Vector2i& opDivAssign(int rhs)", asMETHOD(Vector2i, operator/=), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("Vector2i", "bool opEquals(const Vector2i &in other)", asMETHOD(Vector2i, operator==), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("Vector2i", "Vector2i opAdd(const Vector2i &in other)", asMETHOD(Vector2i, operator+), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("Vector2i", "Vector2i opSub(const Vector2i &in other)", asMETHODPR(Vector2i, operator-, (const Vector2i&) const, Vector2i), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("Vector2i", "Vector2i opMul(int scalar)", asMETHOD(Vector2i, operator*), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("Vector2i", "Vector2i opDiv(int divisor)", asMETHOD(Vector2i, operator/), asCALL_THISCALL));


        AS_VERIFY(engine->RegisterObjectType("IntRect", sizeof(IntRect), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<Vector2i>()));
        AS_VERIFY(engine->RegisterObjectProperty("IntRect", "int left", asOFFSET(IntRect, left)));
        AS_VERIFY(engine->RegisterObjectProperty("IntRect", "int top", asOFFSET(IntRect, top)));
        AS_VERIFY(engine->RegisterObjectProperty("IntRect", "int width", asOFFSET(IntRect, width)));
        AS_VERIFY(engine->RegisterObjectProperty("IntRect", "int height", asOFFSET(IntRect, height)));
        AS_VERIFY(engine->RegisterObjectBehaviour("IntRect", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(angelMath_IntRect_DefaultConstruct), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectBehaviour("IntRect", asBEHAVE_CONSTRUCT, "void f(int, int, int, int)", asFUNCTION(angelMath_IntRect_Construct1), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectBehaviour("IntRect", asBEHAVE_CONSTRUCT, "void f(Vector2i x, Vector2i y)", asFUNCTION(angelMath_IntRect_Construct2), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterGlobalFunction("bool opEquals(const IntRect &in lhs, const IntRect &in rhs)", asFUNCTIONPR(operator==, (const IntRect&, const IntRect&), bool), asCALL_CDECL));
        AS_VERIFY(engine->RegisterObjectMethod("IntRect", "bool intersects(const IntRect &in)", asMETHOD(IntRect, intersects), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("IntRect", "void offset(Vector2i offset)", asMETHOD(IntRect, intersects), asCALL_THISCALL));
    }
}
