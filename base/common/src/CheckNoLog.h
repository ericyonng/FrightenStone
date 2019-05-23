#ifndef CHECK_NO_LOG_H_
#define CHECK_NO_LOG_H_

#pragma once


#undef SAFE_DELETE_NL
#undef SAFE_DELETE_MULTI_NL
#undef SAFE_RELEASE_NL
#undef CLASS_DELETE_SELF_NL
#define SAFE_DELETE_NL(ptr)		{ if(ptr){ try{ delete (ptr); }catch(...){ ASSERT(!(#ptr)); } (ptr) = 0; } }
#define SAFE_DELETE_MULTI_NL(ptr)		{ if(ptr){ try{ delete [](ptr); }catch(...){ ASSERT(!(#ptr)); } (ptr) = 0; } }
#define SAFE_RELEASE_NL(ptr)		{ if(ptr){ try{ (ptr)->Release(); }catch(...){ASSERT(!(#ptr)); } (ptr) = 0; } }
#define CLASS_DELETE_SELF_NL(ptr)	{ if(ptr){ try{ delete (ptr); }catch(...){ ASSERT(!(#ptr)); } } (ptr)=0; }


//еп╤о
#undef CHECKF_NL
#undef CHECK_NL
#undef IF_NOT_NL
#undef IF_OK_NL
#define CHECKF_NL(x)		if(!(x)){ASSERT(!(#x));return false;}
#define CHECK_NL(x)		if(!(x)){ASSERT(!(#x)); return;}
#define IF_NOT_NL(x)		if( !(x)? ( ( ASSERT(!(#x)) ), 1):0 )
#define IF_OK_NL(x)		if( (x)?1:( (ASSERT(!(#x)) ), 0) )

#undef CLASSCHECKF_NL
#undef CLASSCHECK_NL
#undef CLASSIF_NOT_NL
#undef CLASSIF_OK_NL
#define CLASSCHECKF_NL(x)		if(!(x)){ASSERT(!(#x));return false;}
#define CLASSCHECK_NL(x)		if(!(x)){ ASSERT(!(#x)); return;}
#define CLASSIF_NOT_NL(x)		if( !(x)?(ASSERT(!(#x)),1):0 )
#define CLASSIF_OK_NL(x)		if( (x)?1:( (ASSERT(!(#x)), 0) ,0 ) )

#undef  DOTRY_B_NL
#define DOTRY_B_NL		try{

#undef	DOCATCH_E_NL
#undef	DOCATCH_E1_NL
#undef	DOCATCH_E2_NL
#undef	DOCATCH_E3_NL
#undef	DOCATCH_E4_NL
#undef	DOCATCH_E5_NL
#undef	DOCATCH_E6_NL
#undef	DOCATCH_E7_NL
#undef	DOCATCH_E8_NL
#undef	DOCATCH_E9_NL
#undef	DOCATCH_E10_NL
#undef  DOCATCH_ANY_NL
#undef DOCATCH_ANY_NL_END
#define DOCATCH_E_NL		}catch(std::exception &e){ASSERT(!e.what());}catch(...){ASSERT(false);}
#define DOCATCH_E1_NL(x)		}catch(std::exception &e){ e; ASSERT(!(#x));}catch(...){ASSERT(!(#x));}
#define DOCATCH_E2_NL(x, y)		}catch(std::exception &e){ e; ASSERT(!(#x" "#y));}catch(...){ASSERT(!(#x" "#y));}
#define DOCATCH_E3_NL(x, y, z)		}catch(std::exception &e){ e; ASSERT(!(#x" "#y" "#z));}catch(...){ASSERT(!(#x" "#y" "#z));}
#define DOCATCH_E4_NL(x, y, z, a)		}catch(std::exception &e){ e; ASSERT(!(#x" "#y" "#z" "#a));}catch(...){ASSERT(!(#x" "#y" "#z" "#a));}
#define DOCATCH_E5_NL(x, y, z, a, b)		}catch(std::exception &e){ e; ASSERT(!(#x" "#y" "#z" "#a" "#b));}catch(...){ASSERT(!(#x" "#y" "#z" "#a" "#b));}
#define DOCATCH_E6_NL(x, y, z, a, b, c)		}catch(std::exception &e){ e; ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c));}catch(...){ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c));}
#define DOCATCH_E7_NL(x, y, z, a, b, c, d)		}catch(std::exception &e){ e; ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d));}catch(...){ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d));}
#define DOCATCH_E8_NL(x, y, z, a, b, c, d, f)		}catch(std::exception &e){ e; ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f)); }catch(...){ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f));}
#define DOCATCH_E9_NL(x, y, z, a, b, c, d, f, g)		}catch(std::exception &e){ e; ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g)); }catch(...){ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g));}
#define DOCATCH_E10_NL(x, y, z, a, b, c, d, f, g, h)		}catch(std::exception &e){ e; ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g" "#h));}catch(...){ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g" "#h)); }
#define DOCATCH_ANY_NL(x)	}catch(...){ ASSERT(!(#x));
#define DOCATCH_ANY_NL_END	}

#undef	DOCLASSCATCH_E_NL
#undef	DOCLASSCATCH_E1_NL
#undef	DOCLASSCATCH_E2_NL
#undef	DOCLASSCATCH_E3_NL
#undef	DOCLASSCATCH_E4_NL
#undef	DOCLASSCATCH_E5_NL
#undef	DOCLASSCATCH_E6_NL
#undef	DOCLASSCATCH_E7_NL
#undef	DOCLASSCATCH_E8_NL
#undef	DOCLASSCATCH_E9_NL
#undef	DOCLASSCATCH_E10_NL
#undef	DOCLASSCATCH_ANY_NL
#undef	DOCLASSCATCH_ANY_END_NL
#define DOCLASSCATCH_E_NL		}catch(std::exception &e){ASSERT(!e.what());}catch(...){ASSERT(false);}
#define DOCLASSCATCH_E1_NL(x)		}catch(std::exception &e){ e; ASSERT(!(#x));}catch(...){ ASSERT(!(#x));}
#define DOCLASSCATCH_E2_NL(x, y)		}catch(std::exception &e){ e; ASSERT(!(#x" "#y));}catch(...){ ASSERT(!(#x" "#y));}
#define DOCLASSCATCH_E3_NL(x, y, z)		}catch(std::exception &e){ e; ASSERT(!(#x" "#y" "#z));}catch(...){ ASSERT(!(#x" "#y" "#z));}
#define DOCLASSCATCH_E4_NL(x, y, z, a)		}catch(std::exception &e){ e; ASSERT(!(#x" "#y" "#z" "#a));}catch(...){ ASSERT(!(#x" "#y" "#z" "#a));}
#define DOCLASSCATCH_E5_NL(x, y, z, a, b)		}catch(std::exception &e){ e; ASSERT(!(#x" "#y" "#z" "#a" "#b));}catch(...){ ASSERT(!(#x" "#y" "#z" "#a" "#b));}
#define DOCLASSCATCH_E6_NL(x, y, z, a, b, c)		}catch(std::exception &e){ e; ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c));}catch(...){ ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c));}
#define DOCLASSCATCH_E7_NL(x, y, z, a, b, c, d)		}catch(std::exception &e){ e; ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d));}catch(...){ ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d));}
#define DOCLASSCATCH_E8_NL(x, y, z, a, b, c, d, f)		}catch(std::exception &e){ e; ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f));}catch(...){ ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f));}
#define DOCLASSCATCH_E9_NL(x, y, z, a, b, c, d, f, g)		}catch(std::exception &e){ e;  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g)); }catch(...){   ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g)); }
#define DOCLASSCATCH_E10_NL(x, y, z, a, b, c, d, f, g, h)		}catch(std::exception &e){ e; ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g" "#h)); }catch(...){  ASSERT(!(#x" "#y" "#z" "#a" "#b" "#c" "#d" "#f" "#g" "#h)); }
#define DOCLASSCATCH_ANY_NL(x)	}catch(...){  ASSERT(false);
#define DOCLASSCATCH_ANY_END_NL }


#endif

