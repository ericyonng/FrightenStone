#ifndef UNION_PTR_MACRO_H_
#define UNION_PTR_MACRO_H_

#pragma once

#undef DECLEAR_UNION_PTR_BEGIN
#define DECLEAR_UNION_PTR_BEGIN(StructName) \
struct StructName\
{\
union {\
void * m_ptr;

#undef ADD_UNION_PTR_VAR
#define ADD_UNION_PTR_VAR(PtrType, m_pVar_name)\
PtrType * m_pVar_name;

#undef DECLEAR_UNION_PTR_END
#define DECLEAR_UNION_PTR_END(StructName) \
};\
StructName(void *ptr=NULL):m_ptr(ptr){}\
};



#endif

