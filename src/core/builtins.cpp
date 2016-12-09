#include <lispp/builtins.h>

#include <functional>

#include <lispp/objects_all.h>
#include <lispp/list_utils.h>
#include <lispp/scope.h>
#include <lispp/function_utils.h>
#include <lispp/user_callable_object.h>
#include <lispp/virtual_machine.h>

// FIXME: how to split this code to several files?
namespace lispp {
namespace builtins {

ObjectPtr<> cond_macro(const std::shared_ptr<Scope>& scope,
                       const std::vector<ObjectPtr<>>& args) {
  for (auto& branch : args) {
    auto cons_branch = arg_cast<ConsObject>(branch, "cond");

    auto unpacked_branch = unpack_list(cons_branch);
    ObjectPtr<> condition = unpacked_branch[0];
    ObjectPtr<> action = unpacked_branch[1];

    if (is_true_condition(condition, scope)) {
      return action->eval(scope);
    }
  }

  return nullptr;
}

ObjectPtr<> if_macro(const std::shared_ptr<Scope>& scope,
                     const std::vector<ObjectPtr<>>& args) {
  check_args_count("if", args.size(), 2, 3, CallableType::kMacro);

  ObjectPtr<> condition = args[0];
  if (is_true_condition(condition, scope)) {
    return args[1].safe_eval(scope);
  } else if (args.size() == 3) {
    return args[2].safe_eval(scope);
  } else {
    return nullptr;
  }
}

ObjectPtr<> quote_macro(const std::shared_ptr<Scope>&,
                    const std::vector<ObjectPtr<>>& args) {
  check_args_count("quote", args.size(), 1, CallableType::kMacro);

  return args[0];
}

ObjectPtr<> eval_macro(const std::shared_ptr<Scope>& scope,
                   const std::vector<ObjectPtr<>>& args) {
  check_args_count("eval", args.size(), 1, CallableType::kMacro);

  return args[0].safe_eval(scope).safe_eval(scope);
}

ObjectPtr<> not_macro(const std::shared_ptr<Scope>& scope,
                  const std::vector<ObjectPtr<>>& args) {
  check_args_count("not", args.size(), 1, CallableType::kMacro);

  bool condition_value = is_true_condition(args[0], scope);
  return new BooleanObject(!condition_value);
}

ObjectPtr<> or_macro(const std::shared_ptr<Scope>& scope,
                 const std::vector<ObjectPtr<>>& args) {
  ObjectPtr<> result(new BooleanObject(false));
  for (std::size_t arg_index = 0;
       arg_index < args.size() && !is_true_value(result);
       ++arg_index) {
    result = args[arg_index].safe_eval(scope);
  }

  return result;
}

ObjectPtr<> and_macro(const std::shared_ptr<Scope>& scope,
                  const std::vector<ObjectPtr<>>& args) {
  ObjectPtr<> result(new BooleanObject(true));
  for (std::size_t arg_index = 0;
       arg_index < args.size() && is_true_value(result);
       ++arg_index) {
    result = args[arg_index].safe_eval(scope);
  }

  return result;
}

ObjectPtr<> let_macro(const std::shared_ptr<Scope>& scope,
                  const std::vector<ObjectPtr<>>& args) {
  check_args_count("let", args.size(), 1, kInfiniteArgs, CallableType::kMacro);

  std::shared_ptr<Scope> local_scope = std::make_shared<Scope>(scope);
  auto varlist = unpack_list(args[0]);
  for (std::size_t var_index = 0; var_index < varlist.size(); ++var_index) {
    auto var_info = unpack_list(varlist[var_index]);
    if (var_info.size() != 2) {
      throw ExecutionError("let macro expects exactly 2 values"
                           "for variable item");
    }

    auto varname = arg_cast<SymbolObject>(var_info[0], "let");
    ObjectPtr<> eval_result = var_info[1].safe_eval(scope);

    local_scope->set_value(varname->get_value(), eval_result);
  }

  ObjectPtr<> result;
  for (std::size_t expr_index = 1; expr_index < args.size(); ++expr_index) {
    result = args[expr_index].safe_eval(local_scope);
  }

  return result;
}

namespace {
  void parse_callable_definition(const std::string& macro_name,
                                 ObjectPtr<> cons_args,
                                 std::vector<std::string>* arg_names,
                                 std::string* rest_arg_name) {
    std::vector<ObjectPtr<>> arg_symbols;
    auto rest_arg = unpack_list_rest(cons_args, &arg_symbols);
    arg_names->reserve(arg_symbols.size());
    for (std::size_t arg_index = 0; arg_index < arg_symbols.size(); ++arg_index) {
      auto sym_arg = arg_cast<SymbolObject>(arg_symbols[arg_index], macro_name);
      arg_names->push_back(sym_arg->get_value());
    }

    if (rest_arg.valid()) {
      auto rest_arg_symbol = arg_cast<SymbolObject>(rest_arg, macro_name);
      *rest_arg_name = rest_arg_symbol->get_value();
    } else {
      *rest_arg_name = "";
    }
  }
} // namespace

ObjectPtr<> lambda_macro(const std::shared_ptr<Scope>& scope,
                         const std::vector<ObjectPtr<>>& args) {
  check_args_count("lambda", args.size(), 2, kInfiniteArgs,
                   CallableType::kMacro);

  std::vector<std::string> arg_names;
  std::string rest_arg_name;
  parse_callable_definition("lambda", args[0], &arg_names, &rest_arg_name);
  std::vector<ObjectPtr<>> body(std::next(args.begin()), args.end());

  static int lambda_number = 0;
  std::stringstream name_ss;
  name_ss << "<lambda#" << lambda_number++ << ">";

  return new UserCallableObject(name_ss.str(), arg_names, body,
                                scope, rest_arg_name);
}

namespace {

  ObjectPtr<> define_callable(const std::string& macro_name,
                              const std::shared_ptr<Scope>& scope,
                              const std::vector<ObjectPtr<>>& args,
                              CallableType callable_type) {
    auto header = arg_cast<ConsObject>(args[0], macro_name);

    auto function_name = arg_cast<SymbolObject>(
        header->get_left_value(), macro_name);
    auto function_args = header->get_right_value();

    std::vector<std::string> arg_names;
    std::string rest_arg_name;
    parse_callable_definition(macro_name, function_args,
                              &arg_names, &rest_arg_name);

    std::vector<ObjectPtr<>> body(std::next(args.begin()), args.end());

    ObjectPtr<CallableObject> result(new UserCallableObject(
        function_name->get_value(), arg_names, body, scope,
        rest_arg_name, callable_type));

    scope->set_value(function_name->get_value(), result);
    return result;
  }

} // namespace

ObjectPtr<> define_macro(const std::shared_ptr<Scope>& scope,
                     const std::vector<ObjectPtr<>>& args) {
  check_args_count("define", args.size(), 2, kInfiniteArgs,
                   CallableType::kMacro);

  auto varname_symbol = args[0].safe_cast<SymbolObject>();
  if (varname_symbol.valid()) {
    check_args_count("define", args.size(), 2, CallableType::kMacro);

    auto result = args[1].safe_eval(scope);
    scope->set_value(varname_symbol->get_value(), result);
    return result;
  } else {
    return define_callable("define", scope, args, CallableType::kFunction);
  }
}

ObjectPtr<> defmacro_macro(const std::shared_ptr<Scope>& scope,
                       const std::vector<ObjectPtr<>>& args) {
  check_args_count("define-macro", args.size(), 1, kInfiniteArgs,
                   CallableType::kMacro);

  define_callable("define-macro", scope, args, CallableType::kMacro);
  return nullptr;
}

ObjectPtr<> set_macro(const std::shared_ptr<Scope>& scope,
                  const std::vector<ObjectPtr<>>& args) {
  check_args_count("set!", args.size(), 2, CallableType::kMacro);

  auto sym_name = arg_cast<SymbolObject>(args[0], "set!");

  auto eval_result = args[1].safe_eval(scope);
  scope->replace_value(sym_name->get_value(), eval_result);

  return nullptr;
}

ObjectPtr<> setcar_macro(const std::shared_ptr<Scope>& scope,
                         const std::vector<ObjectPtr<>>& args) {
  check_args_count("set-car!", args.size(), 2, CallableType::kMacro);

  auto sym_name = arg_cast<SymbolObject>(args[0], "set-car!");

  auto object = scope->get_value(sym_name->get_value()).safe_cast<ConsObject>();
  if (!object.valid()) {
    throw ExecutionError("Variable of set-car! must be a cons");
  }

  object->set_left_value(args[1].safe_eval(scope));

  return object;
}

ObjectPtr<> setcdr_macro(const std::shared_ptr<Scope>& scope,
                         const std::vector<ObjectPtr<>>& args) {
  check_args_count("set-cdr!", args.size(), 2, CallableType::kMacro);

  auto sym_name = arg_cast<SymbolObject>(args[0], "set-cdr!");

  auto object = scope->get_value(sym_name->get_value()).safe_cast<ConsObject>();
  if (!object.valid()) {
    throw ExecutionError("Variable of set-cdr! must be a cons");
  }

  object->set_right_value(args[1].safe_eval(scope));

  return object;
}

ObjectPtr<> cons_function(const std::shared_ptr<Scope>&,
                          const std::vector<ObjectPtr<>>& args) {
  check_args_count("cons", args.size(), 2, CallableType::kMacro);

  ObjectPtr<> result(new ConsObject(args[0], args[1]));
  return result;
}

ObjectPtr<> car_function(const std::shared_ptr<Scope>&,
                         const std::vector<ObjectPtr<>>& args) {
  check_args_count("car", args.size(), 1, CallableType::kMacro);

  auto cons = arg_cast<ConsObject>(args[0], "car", 0);
  return cons->get_left_value();
}

ObjectPtr<> cdr_function(const std::shared_ptr<Scope>&,
                         const std::vector<ObjectPtr<>>& args) {
  check_args_count("cdr", args.size(), 1, CallableType::kMacro);

  auto cons = arg_cast<ConsObject>(args[0], "cdr", 0);
  return cons->get_right_value();
}

ObjectPtr<> nullp_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args) {
  check_args_count("null?", args.size(), 1);

  return new BooleanObject(!args[0].valid());
}

ObjectPtr<> numberp_function(const std::shared_ptr<Scope>&,
                             const std::vector<ObjectPtr<>>& args) {
  check_args_count("number?", args.size(), 1);

  bool result_value = args[0].safe_cast<NumberObject>().valid();
  return new BooleanObject(result_value);
}

ObjectPtr<> booleanp_function(const std::shared_ptr<Scope>&,
                              const std::vector<ObjectPtr<>>& args) {
  check_args_count("boolean?", args.size(), 1);

  bool result_value = args[0].safe_cast<BooleanObject>().valid();
  return new BooleanObject(result_value);
}

ObjectPtr<> consp_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args) {
  check_args_count("cons?", args.size(), 1);

  bool result_value = args[0].safe_cast<ConsObject>().valid();
  return new BooleanObject(result_value);
}

ObjectPtr<> listp_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args) {
  check_args_count("list?", args.size(), 1);

  bool result = true;
  ObjectPtr<> tail = args[0];
  while (result && tail.valid()) {
    auto cons_tail = tail.safe_cast<ConsObject>();
    if (cons_tail.valid()) {
      tail = cons_tail->get_right_value();
    } else {
      result = false;
    }
  }
  return new BooleanObject(result);
}

ObjectPtr<> symbolp_function(const std::shared_ptr<Scope>&,
                             const std::vector<ObjectPtr<>>& args) {
  check_args_count("symbol?", args.size(), 1);

  bool result_value = args[0].safe_cast<SymbolObject>().valid();
  return new BooleanObject(result_value);
}

ObjectPtr<> stringp_function(const std::shared_ptr<Scope>&,
                             const std::vector<ObjectPtr<>>& args) {
  check_args_count("string?", args.size(), 1);

  bool result_value = args[0].safe_cast<CharactersObject>().valid();
  return new BooleanObject(result_value);
}

ObjectPtr<> plus_function(const std::shared_ptr<Scope>&,
                          const std::vector<ObjectPtr<>>& args) {
  double result = 0;
  for (std::size_t arg_index = 0; arg_index < args.size(); ++arg_index) {
    auto number = arg_cast<NumberObject>(args[arg_index], "+", arg_index);

    result += number->get_value();
  }

  return new NumberObject(result);
}

ObjectPtr<> minus_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args) {
  if (args.empty()) {
    throw ExecutionError("- requires at least one argument");
  }

  auto first_number = arg_cast<NumberObject>(args[0], "-", 0);
  double result = first_number->get_value();

  if (args.size() == 1) {
    return new NumberObject(-result);
  }

  for (std::size_t arg_index = 1; arg_index < args.size(); ++arg_index) {
    auto number = arg_cast<NumberObject>(args[arg_index], "-", arg_index);

    result -= number->get_value();
  }

  return new NumberObject(result);
}

ObjectPtr<> mul_function(const std::shared_ptr<Scope>&,
                         const std::vector<ObjectPtr<>>& args) {
  double result = 1;
  for (std::size_t arg_index = 0; arg_index < args.size(); ++arg_index) {
    auto number = arg_cast<NumberObject>(args[arg_index], "*", arg_index);

    result *= number->get_value();
  }

  return new NumberObject(result);
}

ObjectPtr<> div_function(const std::shared_ptr<Scope>&,
                         const std::vector<ObjectPtr<>>& args) {
  if (args.empty()) {
    throw ExecutionError("/ requires at least one argument");
  }

  auto first_number = arg_cast<NumberObject>(args[0], "/", 0);
  double result = first_number->get_value();

  if (args.size() == 1) {
    return new NumberObject(1 / result);
  }

  for (std::size_t arg_index = 1; arg_index < args.size(); ++arg_index) {
    auto number = arg_cast<NumberObject>(args[arg_index], "/", arg_index);

    result /= number->get_value();
  }

  return new NumberObject(result);
}

// FIXME: move to header?
template<typename ObjectType, typename Comparator>
struct CompareFunc {
  CompareFunc(const std::string& comp_name)
      : comp_name(comp_name) {}

  ObjectPtr<> operator()(const std::shared_ptr<Scope>&,
                         const std::vector<ObjectPtr<>>& args) const {
    if (args.empty()) {
      return new BooleanObject(true);
    }

    check_args_count(comp_name, args.size(), 2, kInfiniteArgs);

    Comparator comparator;

    bool result = true;
    auto last_obj = arg_cast<ObjectType>(args[0], comp_name, 0);
    for (std::size_t arg_index = 1;
         arg_index < args.size() && result;
         ++arg_index) {
      auto curr_obj = arg_cast<ObjectType>(args[arg_index],
                                           comp_name, arg_index);

      result = result && comparator(last_obj->get_value(),
                                    curr_obj->get_value());
      last_obj = curr_obj;
    }

    return new BooleanObject(result);
  }

  std::string comp_name;
};

ObjectPtr<> string_len_function(const std::shared_ptr<Scope>&,
                                const std::vector<ObjectPtr<>>& args) {
  check_args_count("string-length", args.size(), 1);
  auto string = arg_cast<CharactersObject>(args[0], "string-length");

  return new NumberObject(string->get_value().size());
}

ObjectPtr<> print_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args) {
  for (auto& object : args) {
    if (object.valid()) {
      std::cout << *object;
    } else {
      std::cout << "nil";
    }
    std::cout << std::endl;
  }

  return nullptr;
}

ObjectPtr<> exit_function(const std::shared_ptr<Scope>&,
                          const std::vector<ObjectPtr<>>& args) {
  int code = 0;
  if (args.size() > 0) {
    auto number = args[0].safe_cast<NumberObject>();

    if (number.valid()) {
      code = static_cast<int>(number->get_value());
    }
  }

  exit(code);
  return nullptr;
}

ObjectPtr<> throw_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args) {
  std::stringstream ss;
  ss << "Throw from code: ";
  for (auto& arg : args) {
    ss << (arg.valid() ? arg->to_string() : "nil") << " ";
  }

  throw ExecutionError(ss.str());
}

extern const char* kBuiltinsStdlib_common;

} // builtins

void init_global_scope(const std::shared_ptr<Scope>& scope) {
  init_scope_with_builtins(scope);
  init_scope_with_stdlibs(scope);
}

void init_scope_with_builtins(const std::shared_ptr<Scope>& scope) {
  using namespace builtins;

  // Built-in macro
  static ObjectPtr<CallableObject> cond(
      make_simple_callable(cond_macro, CallableType::kMacro));
  scope->set_value("cond", cond);

  static ObjectPtr<CallableObject> if_(
      make_simple_callable(if_macro, CallableType::kMacro));
  scope->set_value("if", if_);

  static ObjectPtr<CallableObject> quote(
      make_simple_callable(quote_macro, CallableType::kMacro));
  scope->set_value("quote", quote);

  static ObjectPtr<CallableObject> eval(
      make_simple_callable(eval_macro, CallableType::kMacro));
  scope->set_value("eval", eval);

  static ObjectPtr<CallableObject> let(
      make_simple_callable(let_macro, CallableType::kMacro, true));
  scope->set_value("let", let);

  static ObjectPtr<CallableObject> lambda(
      make_simple_callable(lambda_macro, CallableType::kMacro));
  scope->set_value("lambda", lambda);

  static ObjectPtr<CallableObject> define(
      make_simple_callable(define_macro, CallableType::kMacro));
  scope->set_value("define", define);

  static ObjectPtr<CallableObject> defmacro(
      make_simple_callable(defmacro_macro, CallableType::kMacro));
  scope->set_value("define-macro", defmacro);

  static ObjectPtr<CallableObject> set_(
      make_simple_callable(set_macro, CallableType::kMacro));
  scope->set_value("set!", set_);

  static ObjectPtr<CallableObject> setcar(
      make_simple_callable(setcar_macro, CallableType::kMacro));
  scope->set_value("set-car!", setcar);

  static ObjectPtr<CallableObject> setcdr(
      make_simple_callable(setcdr_macro, CallableType::kMacro));
  scope->set_value("set-cdr!", setcdr);

  // Boolean macroses
  static ObjectPtr<CallableObject> not_(
      make_simple_callable(not_macro, CallableType::kMacro));
  scope->set_value("not", not_);

  static ObjectPtr<CallableObject> or_(
      make_simple_callable(or_macro, CallableType::kMacro));
  scope->set_value("or", or_);

  static ObjectPtr<CallableObject> and_(
      make_simple_callable(and_macro, CallableType::kMacro));
  scope->set_value("and", and_);

  // List operators
  static ObjectPtr<CallableObject> cons(make_simple_callable(cons_function));
  scope->set_value("cons", cons);

  static ObjectPtr<CallableObject> car(make_simple_callable(car_function));
  scope->set_value("car", car);

  static ObjectPtr<CallableObject> cdr(make_simple_callable(cdr_function));
  scope->set_value("cdr", cdr);

  // Built-in predicates
  static ObjectPtr<CallableObject> nullp(make_simple_callable(nullp_function));
  scope->set_value("null?", nullp);

  static ObjectPtr<CallableObject> numberp(
      make_simple_callable(numberp_function));
  scope->set_value("number?", numberp);

  static ObjectPtr<CallableObject> booleanp(
      make_simple_callable(booleanp_function));
  scope->set_value("boolean?", booleanp);

  static ObjectPtr<CallableObject> consp(make_simple_callable(consp_function));
  scope->set_value("cons?", consp);

  static ObjectPtr<CallableObject> listp(make_simple_callable(listp_function));
  scope->set_value("list?", listp);

  static ObjectPtr<CallableObject> symbolp(
      make_simple_callable(symbolp_function));
  scope->set_value("symbol?", symbolp);

  static ObjectPtr<CallableObject> stringp(
      make_simple_callable(stringp_function));
  scope->set_value("string?", stringp);

  // Number operators
  static ObjectPtr<CallableObject> plus(make_simple_callable(plus_function));
  scope->set_value("+", plus);

  static ObjectPtr<CallableObject> minus(make_simple_callable(minus_function));
  scope->set_value("-", minus);

  static ObjectPtr<CallableObject> mul(make_simple_callable(mul_function));
  scope->set_value("*", mul);

  static ObjectPtr<CallableObject> div(make_simple_callable(div_function));
  scope->set_value("/", div);

  static ObjectPtr<CallableObject> less(
      make_simple_callable(CompareFunc<NumberObject,
                                       std::less<double>>("<")));
  scope->set_value("<", less);

  static ObjectPtr<CallableObject> less_equal(
      make_simple_callable(CompareFunc<NumberObject,
                                       std::less_equal<double>>("<=")));
  scope->set_value("<=", less_equal);

  static ObjectPtr<CallableObject> greater(
      make_simple_callable(CompareFunc<NumberObject,
                                       std::greater<double>>(">")));
  scope->set_value(">", greater);

  static ObjectPtr<CallableObject> greater_equal(
      make_simple_callable(CompareFunc<NumberObject,
                                       std::greater_equal<double>>(">=")));
  scope->set_value(">=", greater_equal);

  static ObjectPtr<CallableObject> equal(
      make_simple_callable(CompareFunc<NumberObject,
                                       std::equal_to<double>>("=")));
  scope->set_value("=", equal);

  // Characters operations
  static ObjectPtr<CallableObject> string_len(
      make_simple_callable(string_len_function));
  scope->set_value("string-length", string_len);

  static ObjectPtr<CallableObject> less_chars(
      make_simple_callable(
          CompareFunc<CharactersObject, std::less<std::string>>("string<?")));
  scope->set_value("string<?", less_chars);

  static ObjectPtr<CallableObject> less_equal_chars(
      make_simple_callable(
          CompareFunc<CharactersObject,
                      std::less_equal<std::string>>("string<=?")));
  scope->set_value("string<=?", less_equal_chars);

  static ObjectPtr<CallableObject> greater_chars(
      make_simple_callable(
          CompareFunc<CharactersObject,
                      std::greater<std::string>>("string>?")));
  scope->set_value("string>?", greater_chars);

  static ObjectPtr<CallableObject> greater_equal_chars(
      make_simple_callable(
          CompareFunc<CharactersObject,
                      std::greater_equal<std::string>>("string>=?")));
  scope->set_value("string>=?", greater_equal_chars);

  static ObjectPtr<CallableObject> equal_chars(
      make_simple_callable(
          CompareFunc<CharactersObject,
                      std::equal_to<std::string>>("string=?")));
  scope->set_value("string=?", equal_chars);

  // Misc
  static ObjectPtr<CallableObject> print(make_simple_callable(print_function));
  scope->set_value("print", print);

  static ObjectPtr<CallableObject> exit(make_simple_callable(exit_function));
  scope->set_value("exit", exit);

  static ObjectPtr<CallableObject> throw_(make_simple_callable(throw_function));
  scope->set_value("throw", throw_);

  scope->set_value("null", nullptr);
}

void init_scope_with_stdlibs(const std::shared_ptr<Scope>& scope) {
  VirtualMachine<> vm(scope);
  vm.eval_all(builtins::kBuiltinsStdlib_common);
}

} // lispp
