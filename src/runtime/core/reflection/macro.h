#pragma once

#ifdef REFLECTION_HEADER_TOOL

#define __MAKE_ATTRIBUTE(...) __attribute__((annotate(#__VA_ARGS__)))

#define CLASS(...) __MAKE_ATTRIBUTE(__VA_ARGS__)
#define PROPERTY(...) __MAKE_ATTRIBUTE(__VA_ARGS__)
#define FUNCTION(...) __MAKE_ATTRIBUTE(__VA_ARGS__)
#define ENUM(...) __MAKE_ATTRIBUTE(__VA_ARGS__)
#define PARAM(...) __MAKE_ATTRIBUTE(__VA_ARGS__)

#define INTERFACE(...) CLASS(__VA_ARGS__)
#define STRUCT(...) CLASS(__VA_ARGS__)

#else

#define __MAKE_ATTRIBUTE(...)

#define CLASS(...) __MAKE_ATTRIBUTE(__VA_ARGS__)
#define PROPERTY(...) __MAKE_ATTRIBUTE(__VA_ARGS__)
#define FUNCTION(...) __MAKE_ATTRIBUTE(__VA_ARGS__)
#define ENUM(...) __MAKE_ATTRIBUTE(__VA_ARGS__)
#define PARAM(...) __MAKE_ATTRIBUTE(__VA_ARGS__)

#define INTERFACE(...) CLASS(__VA_ARGS__)
#define STRUCT(...) CLASS(__VA_ARGS__)

#endif

#define __REFLECTION_BODY(type_name) \
    friend class Serializer; \
    friend class __ReflectionType##type_name;

#define REFLECTION_CLASS_BODY(type_name) __REFLECTION_BODY(type_name)
#define REFLECTION_STRUCT_BODY(type_name) __REFLECTION_BODY(type_name)
#define REFLECTION_INTERFACE_BODY(type_name) __REFLECTION_BODY(type_name)

