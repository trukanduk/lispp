#pragma once

#include <lispp/callable_object.h>
#include <lispp/simple_callable_object.h>
#include <lispp/scope.h>


// TODO: split this file into sections
namespace lispp {
namespace builtins {

// Basic macro
ObjectPtr<> cond_macro(const std::shared_ptr<Scope>& scope,
                       const std::vector<ObjectPtr<>>& args);

ObjectPtr<> if_macro(const std::shared_ptr<Scope>& scope,
                     const std::vector<ObjectPtr<>>& args);

ObjectPtr<> quote_macro(const std::shared_ptr<Scope>&,
                        const std::vector<ObjectPtr<>>& args);

ObjectPtr<> eval_macro(const std::shared_ptr<Scope>& scope,
                       const std::vector<ObjectPtr<>>& args);

ObjectPtr<> not_macro(const std::shared_ptr<Scope>& scope,
                      const std::vector<ObjectPtr<>>& args);

ObjectPtr<> or_macro(const std::shared_ptr<Scope>& scope,
                     const std::vector<ObjectPtr<>>& args);

ObjectPtr<> and_macro(const std::shared_ptr<Scope>& scope,
                      const std::vector<ObjectPtr<>>& args);

ObjectPtr<> let_macro(const std::shared_ptr<Scope>& scope,
                      const std::vector<ObjectPtr<>>& args);

// function & macro definning,
ObjectPtr<> lambda_macro(const std::shared_ptr<Scope>& scope,
                         const std::vector<ObjectPtr<>>& args);

ObjectPtr<> define_macro(const std::shared_ptr<Scope>& scope,
                         const std::vector<ObjectPtr<>>& args);

ObjectPtr<> defmacro_macro(const std::shared_ptr<Scope>& scope,
                           const std::vector<ObjectPtr<>>& args);

// setters
ObjectPtr<> set_macro(const std::shared_ptr<Scope>& scope,
                      const std::vector<ObjectPtr<>>& args);

ObjectPtr<> setcar_macro(const std::shared_ptr<Scope>& scope,
                         const std::vector<ObjectPtr<>>& args);

ObjectPtr<> setcdr_macro(const std::shared_ptr<Scope>& scope,
                         const std::vector<ObjectPtr<>>& args);

// list operations
ObjectPtr<> cons_function(const std::shared_ptr<Scope>&,
                          const std::vector<ObjectPtr<>>& args);

ObjectPtr<> car_function(const std::shared_ptr<Scope>&,
                         const std::vector<ObjectPtr<>>& args);

ObjectPtr<> cdr_function(const std::shared_ptr<Scope>&,
                         const std::vector<ObjectPtr<>>& args);

// type prediates
ObjectPtr<> nullp_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args);

ObjectPtr<> numberp_function(const std::shared_ptr<Scope>&,
                             const std::vector<ObjectPtr<>>& args);

ObjectPtr<> booleanp_function(const std::shared_ptr<Scope>&,
                              const std::vector<ObjectPtr<>>& args);

ObjectPtr<> consp_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args);

ObjectPtr<> listp_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args);

ObjectPtr<> symbolp_function(const std::shared_ptr<Scope>&,
                             const std::vector<ObjectPtr<>>& args);

ObjectPtr<> stringp_function(const std::shared_ptr<Scope>&,
                             const std::vector<ObjectPtr<>>& args);

// number actions
ObjectPtr<> plus_function(const std::shared_ptr<Scope>&,
                          const std::vector<ObjectPtr<>>& args);

ObjectPtr<> minus_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args);

ObjectPtr<> mul_function(const std::shared_ptr<Scope>&,
                         const std::vector<ObjectPtr<>>& args);

ObjectPtr<> div_function(const std::shared_ptr<Scope>&,
                         const std::vector<ObjectPtr<>>& args);

// misc functions
ObjectPtr<> string_len_function(const std::shared_ptr<Scope>&,
                                const std::vector<ObjectPtr<>>& args);

ObjectPtr<> print_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args);

ObjectPtr<> exit_function(const std::shared_ptr<Scope>&,
                          const std::vector<ObjectPtr<>>& args);

ObjectPtr<> throw_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args);
} // builtins

void init_global_scope(const std::shared_ptr<Scope>& scope);

void init_scope_with_builtins(const std::shared_ptr<Scope>& scope);
void init_scope_with_stdlibs(const std::shared_ptr<Scope>& scope);

} // lispp
