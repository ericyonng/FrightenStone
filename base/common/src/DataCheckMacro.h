#ifndef DATA_CHECK_MACRO_H_
#define DATA_CHECK_MACRO_H_

#pragma once
#include <BaseCode/MyBaseMacro.h>
#include <BaseCode/LogFileDef.h>
//#include <BaseCode/CrashDump.inl>
// 
// #undef CRASH_SAFE_DELETE_LOG
// #undef CRASH_SAFE_RELEASE_LOG
// #undef CRASH_CLASS_SELF_DELETE_LOG
// #define CRASH_SAFE_DELETE_LOG(x)	{LOGCRASH("CATCH *** SAFE_DELETE CRASH! *** [%s]", #x);}
// #define CRASH_SAFE_RELEASE_LOG(x)	{LOGCRASH("CATCH *** SAFE_RELEASE CRASH! *** [%s]", #x);}
// #define CRASH_CLASS_SELF_DELETE_LOG(x)	{LOGCRASH("CATCH *** CLASS_SELF_DELETE CRASH! *** [%s]", #x);}
// #undef SAFE_DELETE
// #undef SAFE_DELETE_MULTI
// #undef SAFE_RELEASE
// #undef CLASS_DELETE_SELF
// #define SAFE_DELETE(ptr)		{ if(ptr){ try{ delete (ptr); }catch(...){ CRASH_SAFE_DELETE_LOG(ptr); ASSERT(!(#ptr)); } (ptr) = 0; } }
// #define SAFE_DELETE_MULTI(ptr)		{ if(ptr){ try{ delete [](ptr); }catch(...){ CRASH_SAFE_DELETE_LOG(ptr); ASSERT(!(#ptr)); } (ptr) = 0; } }
// #define SAFE_RELEASE(ptr)		{ if(ptr){ try{ (ptr)->Release(); }catch(...){CRASH_SAFE_RELEASE_LOG(ptr); ASSERT(!(#ptr)); } (ptr) = 0; } }
// #define CLASS_DELETE_SELF(ptr)	{ if(ptr){ try{ delete [](ptr); }catch(...){CRASH_CLASS_SELF_DELETE_LOG(ptr); ASSERT(!(#ptr)); } }(ptr)=0; }
// 
// //еп╤о
// #undef CHECKF
// #undef CHECK
// #undef IF_NOT
// #undef IF_OK
// #undef ASSERTEX
// 
// #define ASSERTEX(x)		{ if(!(x)){LOGERR("ASSERTEX(%s)",#x); ASSERT(!(#x));} }
// #define CHECKF(x)		if(!(x)){LOGERR("CHECKF(%s)", #x); ASSERT(!(#x));return false;}
// #define CHECK(x)		if(!(x)){LOGERR("CHECK(%s)", #x); ASSERT(!(#x)); return;}
// #define IF_NOT(x)		if( !(x)? ( (LOGERR("IF_NOT(%s)", #x), ASSERT(!(#x)) ), 1):0 )
// #define IF_OK(x)		if( (x)?1:( ( LOGERR("IF_OK(%s)", #x), ASSERT(!(#x)) ), 0) )
// 
// 
// #undef CLASSCHECKF
// #undef CLASSCHECK
// #undef CLASSIF_NOT
// #undef CLASSIF_OK
// #define CLASSCHECKF(x)		if(!(x)){LOGERR("CHECKF(%s) class:%s", #x, typeid(*this).name()); ASSERT(!(#x));return false;}
// #define CLASSCHECK(x)		if(!(x)){LOGERR("CHECK(%s) class:%s", #x, typeid(*this).name()); ASSERT(!(#x)); return;}
// #define CLASSIF_NOT(x)		if( !(x)?( (LOGERR("IF_NOT(%s) class:%s", #x, typeid(*this).name()))?ASSERT(!(#x)):1,1):0 )
// #define CLASSIF_OK(x)		if( (x)?1:( (LOGERR("IF_OK(%s) class:%s", #x, typeid(*this).name())?ASSERT(!(#x)):0) ,0 ) )
// 
// #undef  DOTRY_B
// #define DOTRY_B		try{
// 
// #undef	DOCATCH_E
// #undef	DOCATCH_E1
// #undef	DOCATCH_E2
// #undef	DOCATCH_E3
// #undef	DOCATCH_E4
// #undef	DOCATCH_E5
// #undef	DOCATCH_E6
// #undef	DOCATCH_E7
// #undef	DOCATCH_E8
// #undef	DOCATCH_E9
// #undef	DOCATCH_E10
// #undef  DOCATCH_ANY
// #undef	DOCATCH_ANY_END
// #define DOCATCH_E		}catch(std::exception &e){LOGCRASH("%s", e.what()); ASSERT(false);}catch(...){LOGCRASH("%s","***crash***catch(...)"); ASSERT(false); }
// #define DOCATCH_E1(x)		}catch(std::exception &e){LOGCRASH("%s %s:", e.what(), #x, (x));ASSERT(!(#x));}catch(...){LOGCRASH("***crash***catch(...): %s:", #x, (x) );ASSERT(!(#x));}
// #define DOCATCH_E2(x, y)		}catch(std::exception &e){LOGCRASH("%s %s:", e.what(), #x, (x), " ", #y,":", (y));ASSERT(!(#x" "#y));}catch(...){LOGCRASH("***crash***catch(...): %s", #x,":", (x), " ",#y,":", (y));ASSERT(!(#x" "#y));}
// #define DOCATCH_E3(x, y, z)		}catch(std::exception &e){LOGCRASH("", e.what(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z)); ASSERT(!(#x" "#y" "#z)); }catch(...){LOGCRASH("***crash***catch(...):", " ", #x, ":", (x),  " ",#y,":", (y), " ",#z,":", (z));ASSERT(!(#x" "#y" "#z));}
// #define DOCATCH_E4(x, y, z, a)		}catch(std::exception &e){LOGCRASH("", e.what(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a));ASSERT(!(#x" "#y" "#z" "#a));}catch(...){LOGCRASH("***crash***catch(...):", " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a));ASSERT(!(#x" "#y" "#z" "#a));}
// #define DOCATCH_E5(x, y, z, a, b)		}catch(std::exception &e){LOGCRASH("", e.what(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b));ASSERT(!(#x" "#y" "#z" "#a" "#b));}catch(...){LOGCRASH("***crash***catch(...):"," ",#x,":", (x) , " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b)); ASSERT(!(#x" "#y" "#z" "#a" "#b));}
// #define DOCATCH_E6(x, y, z, a, b, c)		}catch(std::exception &e){LOGCRASH("", e.what(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c)); ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c)); }catch(...){LOGCRASH("***crash***catch(...):", " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c) );  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c));}
// #define DOCATCH_E7(x, y, z, a, b, c, d)		}catch(std::exception &e){LOGCRASH("", e.what(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d));  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d));}catch(...){LOGCRASH("***crash***catch(...):", " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c) , " ",#d,":", (d));  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d));}
// #define DOCATCH_E8(x, y, z, a, b, c, d, f)		}catch(std::exception &e){LOGCRASH("", e.what(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f));  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f));}catch(...){LOGCRASH("***crash***catch(...):", " ",#x,":", (x), " ",#y,":", (y) , " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f));  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f));}
// #define DOCATCH_E9(x, y, z, a, b, c, d, f, g)		}catch(std::exception &e){LOGCRASH("", e.what(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f), " ",#g,":", (g)); ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g)); }catch(...){LOGCRASH("***crash***catch(...):", " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a) , " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f), " ",#g,":", (g)); ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g)); }
// #define DOCATCH_E10(x, y, z, a, b, c, d, f, g, h)		}catch(std::exception &e){LOGCRASH("", e.what(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f), " ",#g,":", (g), " ",#h,":", (h) );  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g" "#h)); }catch(...){LOGCRASH("***crash***catch(...):", " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f), " ",#g,":", (g), " ",#h,":", (h) );  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g" "#h));}
// #define DOCATCH_ANY(x)	}catch(...){LOGCRASH("***crash***catch(...): %s:", #x, (x) );ASSERT(!(#x));
// #define DOCATCH_ANY_END }
// 
// #undef	DOCLASSCATCH_E
// #undef	DOCLASSCATCH_E1
// #undef	DOCLASSCATCH_E2
// #undef	DOCLASSCATCH_E3
// #undef	DOCLASSCATCH_E4
// #undef	DOCLASSCATCH_E5
// #undef	DOCLASSCATCH_E6
// #undef	DOCLASSCATCH_E7
// #undef	DOCLASSCATCH_E8
// #undef	DOCLASSCATCH_E9
// #undef	DOCLASSCATCH_E10
// #undef	DOCLASSCATCH_ANY
// #undef	DOCLASSCATCH_ANY_END
// #define DOCLASSCATCH_E		}catch(std::exception &e){LOGCRASH("", e.what(),"class:",typeid(*this).name()); ASSERT(false); }catch(...){LOGCRASH("***crash***catch(...):","class:",typeid(*this).name()); ASSERT(false); }
// #define DOCLASSCATCH_E1(x)		}catch(std::exception &e){LOGCRASH("", e.what(), "class:",typeid(*this).name()," ",#x,":", (x));  ASSERT(!(#x)); }catch(...){LOGCRASH("***crash***catch(...):","class:",typeid(*this).name(), " ",#x,":", (x) ); ASSERT(!(#x));}
// #define DOCLASSCATCH_E2(x, y)		}catch(std::exception &e){LOGCRASH("", e.what(), "class:",typeid(*this).name()," ",#x,":", (x), " ",#y,":", (y));  ASSERT(!(#x" "#y)); }catch(...){LOGCRASH("***crash***catch(...):","class:",typeid(*this).name(), " ",#x,":", (x), " ",#y,":", (y)); ASSERT(!(#x" "#y)); }
// #define DOCLASSCATCH_E3(x, y, z)		}catch(std::exception &e){LOGCRASH("", e.what(),"class:",typeid(*this).name(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z);  ASSERT(!(#x" "#y" "#z)); }catch(...){LOGCRASH("***crash***catch(...):","class:",typeid(*this).name(), " ",#x,":", (x),  " ",#y,":", (y), " ",#z,":", (z)); ASSERT(!(#x" "#y" "#z)); }
// #define DOCLASSCATCH_E4(x, y, z, a)		}catch(std::exception &e){LOGCRASH("", e.what(),"class:",typeid(*this).name(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a));  ASSERT(!(#x" "#y" "#z" "#a)); }catch(...){LOGCRASH("***crash***catch(...):","class:",typeid(*this).name(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a));  ASSERT(!(#x" "#y" "#z" "#a)); }
// #define DOCLASSCATCH_E5(x, y, z, a, b)		}catch(std::exception &e){LOGCRASH("", e.what(), "class:",typeid(*this).name()," ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b));  ASSERT(!(#x" "#y" "#z" "#a" "#b)); }catch(...){LOGCRASH("***crash***catch(...):","class:",typeid(*this).name(), " ",#x,":", (x) , " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b));  ASSERT(!(#x" "#y" "#z" "#a" "#b));}
// #define DOCLASSCATCH_E6(x, y, z, a, b, c)		}catch(std::exception &e){LOGCRASH("", e.what(), "class:",typeid(*this).name()," ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c));  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c)); }catch(...){LOGCRASH("***crash***catch(...):","class:",typeid(*this).name(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c) );  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c)); }
// #define DOCLASSCATCH_E7(x, y, z, a, b, c, d)		}catch(std::exception &e){LOGCRASH("", e.what(), "class:",typeid(*this).name()," ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d));  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d)); }catch(...){LOGCRASH("***crash***catch(...):","class:",typeid(*this).name(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c) , " ",#d,":", (d)); ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d)); }
// #define DOCLASSCATCH_E8(x, y, z, a, b, c, d, f)		}catch(std::exception &e){LOGCRASH("", e.what(),"class:",typeid(*this).name(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f));  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f)); }catch(...){LOGCRASH("***crash***catch(...):","class:",typeid(*this).name(), " ",#x,":", (x), " ",#y,":", (y) , " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f));  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f)); }
// #define DOCLASSCATCH_E9(x, y, z, a, b, c, d, f, g)		}catch(std::exception &e){LOGCRASH("", e.what(), "class:",typeid(*this).name()," ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f), " ",#g,":", (g));  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g)); }catch(...){LOGCRASH("***crash***catch(...):","class:",typeid(*this).name(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a) , " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f), " ",#g,":", (g));  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g));}
// #define DOCLASSCATCH_E10(x, y, z, a, b, c, d, f, g, h)		}catch(std::exception &e){LOGCRASH("", e.what(), "class:",typeid(*this).name()," ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f), " ",#g,":", (g), " ",#h,":", (h) );  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g" "#h)); }catch(...){LOGCRASH("***crash***catch(...):","class:",typeid(*this).name(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f), " ",#g,":", (g), " ",#h,":", (h) );   ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g" "#h)); }
// #define DOCLASSCATCH_ANY(x)	}catch(...){ LOGCRASH("***crash***catch(...):","class:",typeid(*this).name()); ASSERT(false);
// #define DOCLASSCATCH_ANY_END }


#undef CRASH_SAFE_DELETE_LOG
#undef CRASH_SAFE_RELEASE_LOG
#undef CRASH_CLASS_SELF_DELETE_LOG
#define CRASH_SAFE_DELETE_LOG(x)	{LOGCRASH("CATCH *** SAFE_DELETE CRASH! *** [", #x, "]");}
#define CRASH_SAFE_RELEASE_LOG(x)	{LOGCRASH("CATCH *** SAFE_RELEASE CRASH! *** [", #x, "]");}
#define CRASH_CLASS_SELF_DELETE_LOG(x)	{LOGCRASH("CATCH *** CLASS_SELF_DELETE CRASH! *** [", #x, "]");}
#undef SAFE_DELETE
#undef SAFE_DELETE_MULTI
#undef SAFE_RELEASE
#undef CLASS_DELETE_SELF
//#define SAFE_DELETE(ptr)		{ if(ptr){gtool::CFmtIOString ErrOutput; __try{ delete (ptr); }__except(RecordExceptionInfo(GetExceptionInformation(), "SAFE_DELETE "#ptr,ErrOutput)){CRASH_SAFE_DELETE_LOG(ErrOutput.c_str()); ASSERT(!(#ptr)); } (ptr) = 0; } }
#define SAFE_DELETE(ptr)		{ if(ptr){ try{ delete (ptr); }catch(...){ CRASH_SAFE_DELETE_LOG(ptr); ASSERT(!(#ptr)); } (ptr) = 0; } }

#define SAFE_DELETE_MULTI(ptr)		{ if(ptr){ try{ delete [](ptr); }catch(...){ CRASH_SAFE_DELETE_LOG(ptr); ASSERT(!(#ptr)); } (ptr) = 0; } }
#define SAFE_RELEASE(ptr)		{ if(ptr){ try{ (ptr)->Release(); }catch(...){CRASH_SAFE_RELEASE_LOG(ptr); ASSERT(!(#ptr)); } (ptr) = 0; } }
#define CLASS_DELETE_SELF(ptr)	{ if(ptr){ try{ delete [](ptr); }catch(...){CRASH_CLASS_SELF_DELETE_LOG(ptr); ASSERT(!(#ptr)); } }(ptr)=0; }

//еп╤о
#undef CHECKF
#undef CHECK
#undef IF_NOT
#undef IF_OK
#undef ASSERTEX

#define ASSERTEX(x)		{ if(!(x)){LOGERR("ASSERTEX(",#x, ")"); ASSERT(!(#x));} }
#define CHECKF(x)		if(!(x)){LOGERR("CHECKF(", #x,")"); ASSERT(!(#x));return false;}
#define CHECK(x)		if(!(x)){LOGERR("CHECK(", #x, ")"); ASSERT(!(#x)); return;}
#define IF_NOT(x)		if( !(x)? ( (LOGERR("IF_NOT(", #x, ")"), ASSERT(!(#x)) ), 1):0 )
#define IF_OK(x)		if( (x)?1:( ( LOGERR("IF_OK(", #x, ")"), ASSERT(!(#x)) ), 0) )

#undef DEAD_LOOP_BREAK_EX
#define DEAD_LOOP_BREAK_EX(x, n) \
		if((++(x))>n){ ASSERTEX((x)<=n);break;}

#undef ASSERTNET
#define ASSERTNET(x) { if(!(x)){LOGNET("ASSERTNET(", #x, ")"); ASSERT(!(#x));} }


#undef CLASSCHECKF
#undef CLASSCHECK
#undef CLASSIF_NOT
#undef CLASSIF_OK
#define CLASSCHECKF(x)		if(!(x)){LOGERR("CHECKF(", #x, ")", "class:", typeid(*this).name()); ASSERT(!(#x));return false;}
#define CLASSCHECK(x)		if(!(x)){LOGERR("CHECK(", #x, ")", "class:", typeid(*this).name()); ASSERT(!(#x)); return;}
#define CLASSIF_NOT(x)		if( !(x)?( (LOGERR("IF_NOT(", #x, "class:", typeid(*this).name()))?ASSERT(!(#x)):1,1):0 )
#define CLASSIF_OK(x)		if( (x)?1:( (LOGERR("IF_OK(",#x, "class:", typeid(*this).name())?ASSERT(!(#x)):0) ,0 ) )

#undef  DOTRY_B
#define DOTRY_B		try{

#undef	DOCATCH_E
#undef	DOCATCH_E1
#undef	DOCATCH_E2
#undef	DOCATCH_E3
#undef	DOCATCH_E4
#undef	DOCATCH_E5
#undef	DOCATCH_E6
#undef	DOCATCH_E7
#undef	DOCATCH_E8
#undef	DOCATCH_E9
#undef	DOCATCH_E10
#undef  DOCATCH_ANY
#undef	DOCATCH_ANY_END
#define DOCATCH_E		}catch(std::exception &e){LOGCRASH(e.what()); ASSERT(false);}catch(...){LOGCRASH("***crash***catch(...)"); ASSERT(false); }
#define DOCATCH_E1(x)		}catch(std::exception &e){LOGCRASH(e.what()," ", #x,":", (x));ASSERT(!(#x));}catch(...){LOGCRASH("***crash***catch(...) ", #x, ":", (x) );ASSERT(!(#x));}
#define DOCATCH_E2(x, y)		}catch(std::exception &e){LOGCRASH(e.what()," ", #x,":", (x), " ", #y,":", (y));ASSERT(!(#x" "#y));}catch(...){LOGCRASH("***crash***catch(...): ", #x,":", (x), " ",#y,":", (y));ASSERT(!(#x" "#y));}
#define DOCATCH_E3(x, y, z)		}catch(std::exception &e){LOGCRASH(e.what(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z)); ASSERT(!(#x" "#y" "#z)); }catch(...){LOGCRASH("***crash***catch(...):", " ", #x, ":", (x),  " ",#y,":", (y), " ",#z,":", (z));ASSERT(!(#x" "#y" "#z));}
#define DOCATCH_E4(x, y, z, a)		}catch(std::exception &e){LOGCRASH(e.what(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a));ASSERT(!(#x" "#y" "#z" "#a));}catch(...){LOGCRASH("***crash***catch(...):", " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a));ASSERT(!(#x" "#y" "#z" "#a));}
#define DOCATCH_E5(x, y, z, a, b)		}catch(std::exception &e){LOGCRASH(e.what(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b));ASSERT(!(#x" "#y" "#z" "#a" "#b));}catch(...){LOGCRASH("***crash***catch(...):"," ",#x,":", (x) , " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b)); ASSERT(!(#x" "#y" "#z" "#a" "#b));}
#define DOCATCH_E6(x, y, z, a, b, c)		}catch(std::exception &e){LOGCRASH(e.what(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c)); ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c)); }catch(...){LOGCRASH("***crash***catch(...):", " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c) );  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c));}
#define DOCATCH_E7(x, y, z, a, b, c, d)		}catch(std::exception &e){LOGCRASH(e.what(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d));  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d));}catch(...){LOGCRASH("***crash***catch(...):", " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c) , " ",#d,":", (d));  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d));}
#define DOCATCH_E8(x, y, z, a, b, c, d, f)		}catch(std::exception &e){LOGCRASH(e.what(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f));  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f));}catch(...){LOGCRASH("***crash***catch(...):", " ",#x,":", (x), " ",#y,":", (y) , " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f));  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f));}
#define DOCATCH_E9(x, y, z, a, b, c, d, f, g)		}catch(std::exception &e){LOGCRASH(e.what(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f), " ",#g,":", (g)); ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g)); }catch(...){LOGCRASH("***crash***catch(...):", " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a) , " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f), " ",#g,":", (g)); ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g)); }
#define DOCATCH_E10(x, y, z, a, b, c, d, f, g, h)		}catch(std::exception &e){LOGCRASH(e.what(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f), " ",#g,":", (g), " ",#h,":", (h) );  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g" "#h)); }catch(...){LOGCRASH("***crash***catch(...):", " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f), " ",#g,":", (g), " ",#h,":", (h) );  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g" "#h));}
#define DOCATCH_ANY(x)	}catch(...){LOGCRASH("***crash***catch(...): ", #x,":", (x) );ASSERT(!(#x));
#define DOCATCH_ANY_END }

#undef	DOCLASSCATCH_E
#undef	DOCLASSCATCH_E1
#undef	DOCLASSCATCH_E2
#undef	DOCLASSCATCH_E3
#undef	DOCLASSCATCH_E4
#undef	DOCLASSCATCH_E5
#undef	DOCLASSCATCH_E6
#undef	DOCLASSCATCH_E7
#undef	DOCLASSCATCH_E8
#undef	DOCLASSCATCH_E9
#undef	DOCLASSCATCH_E10
#undef	DOCLASSCATCH_ANY
#undef	DOCLASSCATCH_ANY_END
#define DOCLASSCATCH_E		}catch(std::exception &e){LOGCRASH(e.what(),"class:",typeid(*this).name()); ASSERT(false); }catch(...){LOGCRASH("***crash***catch(...):","class:",typeid(*this).name()); ASSERT(false); }
#define DOCLASSCATCH_E1(x)		}catch(std::exception &e){LOGCRASH( e.what(), "class:",typeid(*this).name()," ",#x,":", (x));  ASSERT(!(#x)); }catch(...){LOGCRASH("***crash***catch(...):","class:",typeid(*this).name(), " ",#x,":", (x) ); ASSERT(!(#x));}
#define DOCLASSCATCH_E2(x, y)		}catch(std::exception &e){LOGCRASH( e.what(), "class:",typeid(*this).name()," ",#x,":", (x), " ",#y,":", (y));  ASSERT(!(#x" "#y)); }catch(...){LOGCRASH("***crash***catch(...):","class:",typeid(*this).name(), " ",#x,":", (x), " ",#y,":", (y)); ASSERT(!(#x" "#y)); }
#define DOCLASSCATCH_E3(x, y, z)		}catch(std::exception &e){LOGCRASH(e.what(),"class:",typeid(*this).name(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z);  ASSERT(!(#x" "#y" "#z)); }catch(...){LOGCRASH("***crash***catch(...):","class:",typeid(*this).name(), " ",#x,":", (x),  " ",#y,":", (y), " ",#z,":", (z)); ASSERT(!(#x" "#y" "#z)); }
#define DOCLASSCATCH_E4(x, y, z, a)		}catch(std::exception &e){LOGCRASH(e.what(),"class:",typeid(*this).name(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a));  ASSERT(!(#x" "#y" "#z" "#a)); }catch(...){LOGCRASH("***crash***catch(...):","class:",typeid(*this).name(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a));  ASSERT(!(#x" "#y" "#z" "#a)); }
#define DOCLASSCATCH_E5(x, y, z, a, b)		}catch(std::exception &e){LOGCRASH(e.what(), "class:",typeid(*this).name()," ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b));  ASSERT(!(#x" "#y" "#z" "#a" "#b)); }catch(...){LOGCRASH("***crash***catch(...):","class:",typeid(*this).name(), " ",#x,":", (x) , " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b));  ASSERT(!(#x" "#y" "#z" "#a" "#b));}
#define DOCLASSCATCH_E6(x, y, z, a, b, c)		}catch(std::exception &e){LOGCRASH(e.what(), "class:",typeid(*this).name()," ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c));  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c)); }catch(...){LOGCRASH("***crash***catch(...):","class:",typeid(*this).name(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c) );  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c)); }
#define DOCLASSCATCH_E7(x, y, z, a, b, c, d)		}catch(std::exception &e){LOGCRASH(e.what(), "class:",typeid(*this).name()," ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d));  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d)); }catch(...){LOGCRASH("***crash***catch(...):","class:",typeid(*this).name(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c) , " ",#d,":", (d)); ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d)); }
#define DOCLASSCATCH_E8(x, y, z, a, b, c, d, f)		}catch(std::exception &e){LOGCRASH(e.what(),"class:",typeid(*this).name(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f));  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f)); }catch(...){LOGCRASH("***crash***catch(...):","class:",typeid(*this).name(), " ",#x,":", (x), " ",#y,":", (y) , " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f));  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f)); }
#define DOCLASSCATCH_E9(x, y, z, a, b, c, d, f, g)		}catch(std::exception &e){LOGCRASH(e.what(), "class:",typeid(*this).name()," ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f), " ",#g,":", (g));  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g)); }catch(...){LOGCRASH("***crash***catch(...):","class:",typeid(*this).name(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a) , " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f), " ",#g,":", (g));  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g));}
#define DOCLASSCATCH_E10(x, y, z, a, b, c, d, f, g, h)		}catch(std::exception &e){LOGCRASH(e.what(), "class:",typeid(*this).name()," ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f), " ",#g,":", (g), " ",#h,":", (h) );  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g" "#h)); }catch(...){LOGCRASH("***crash***catch(...):","class:",typeid(*this).name(), " ",#x,":", (x), " ",#y,":", (y), " ",#z,":", (z), " ",#a,":", (a), " ",#b,":", (b), " ",#c,":", (c), " ",#d,":", (d), " ",#f,":", (f), " ",#g,":", (g), " ",#h,":", (h) );   ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g" "#h)); }
#define DOCLASSCATCH_ANY(x)	}catch(...){ LOGCRASH("***crash***catch(...):","class:",typeid(*this).name()); ASSERT(false);
#define DOCLASSCATCH_ANY_END }

#endif // !MACRO_DEFINE_H_

