
// DO NOT EDIT THIS FILE - it is machine generated -*- c++ -*-

#ifndef __org_omg_CosNaming_NamingContextExtOperations__
#define __org_omg_CosNaming_NamingContextExtOperations__

#pragma interface

#include <java/lang/Object.h>
#include <gcj/array.h>

extern "Java"
{
  namespace org
  {
    namespace omg
    {
      namespace CORBA
      {
          class Object;
      }
      namespace CosNaming
      {
          class BindingIteratorHolder;
          class BindingListHolder;
          class NameComponent;
          class NamingContext;
          class NamingContextExtOperations;
      }
    }
  }
}

class org::omg::CosNaming::NamingContextExtOperations : public ::java::lang::Object
{

public:
  virtual ::org::omg::CORBA::Object * resolve_str(::java::lang::String *) = 0;
  virtual JArray< ::org::omg::CosNaming::NameComponent * > * to_name(::java::lang::String *) = 0;
  virtual ::java::lang::String * to_string(JArray< ::org::omg::CosNaming::NameComponent * > *) = 0;
  virtual ::java::lang::String * to_url(::java::lang::String *, ::java::lang::String *) = 0;
  virtual void bind(JArray< ::org::omg::CosNaming::NameComponent * > *, ::org::omg::CORBA::Object *) = 0;
  virtual void bind_context(JArray< ::org::omg::CosNaming::NameComponent * > *, ::org::omg::CosNaming::NamingContext *) = 0;
  virtual ::org::omg::CosNaming::NamingContext * bind_new_context(JArray< ::org::omg::CosNaming::NameComponent * > *) = 0;
  virtual void destroy() = 0;
  virtual void list(jint, ::org::omg::CosNaming::BindingListHolder *, ::org::omg::CosNaming::BindingIteratorHolder *) = 0;
  virtual ::org::omg::CosNaming::NamingContext * new_context() = 0;
  virtual void rebind(JArray< ::org::omg::CosNaming::NameComponent * > *, ::org::omg::CORBA::Object *) = 0;
  virtual void rebind_context(JArray< ::org::omg::CosNaming::NameComponent * > *, ::org::omg::CosNaming::NamingContext *) = 0;
  virtual ::org::omg::CORBA::Object * resolve(JArray< ::org::omg::CosNaming::NameComponent * > *) = 0;
  virtual void unbind(JArray< ::org::omg::CosNaming::NameComponent * > *) = 0;
  static ::java::lang::Class class$;
} __attribute__ ((java_interface));

#endif // __org_omg_CosNaming_NamingContextExtOperations__