#pragma once

#include <format>
#include <optional>
#include <stdexcept>
#include <string>

#include "Container.h"

class Object {
public:
  Object() { _counter++; }
  virtual ~Object() { _counter--; }
  size_t counter() const { return _counter; }
  virtual std::string toString() const = 0;

private:
  inline static size_t _counter = 0;
};

class Task : public virtual Object {
public:
  virtual void execute() = 0;
  virtual bool has_result() const = 0;
};

class Named : public virtual Object {
public:
  Named() {}
  Named(const std::string &name) : _name(name) {}
  std::string name() const { return _name; }

private:
  std::string _name;
};

enum class Operation : char {
  Add = '+',
  Subtract = '-',
  Multiply = '*',
  Divide = '/'
};

template <typename T> class Binary : public Task, public Named {
public:
  Binary(Operation op, T first_val, T second_val)
      : Named("Binary operation"), _op(op), _first_val(first_val),
        _second_val(second_val) {}

  bool has_result() const override { return true; }

  std::string toString() const override {
    return std::format("{}: {} {} {} = {}", name(), _first_val,
                       static_cast<char>(_op), _second_val, _result.value());
  }

  void execute() override {
    auto operation = static_cast<char>(_op);
    _result = funcs[operation](_first_val, _second_val);
  }

private:
  inline static std::unordered_map<char, std::function<int(int, int)>> funcs = {
      {'+', [](T first, T second) { return first + second; }},
      {'-', [](T first, T second) { return first - second; }},
      {'*', [](T first, T second) { return first * second; }},
      {'/',
       [](T first, T second) {
         if (std::abs(second) > 0) {
           return first / second;
         } else {
           throw std::runtime_error("Zero division");
         }
       }},
  };

private:
  Operation _op;
  T _first_val;
  T _second_val;
  std::optional<T> _result;
};

template <typename T> class Add : public Task {
public:
  Add(std::unique_ptr<T> task, Container<std::unique_ptr<T>> &container)
      : _task(std::move(task)), _container(container) {}

  bool has_result() const override { return false; }

  std::string toString() const override {
    return std::format("Added task in container");
  }

  void execute() override { _container.push_back(std::move(_task)); }

private:
  Container<std::unique_ptr<T>> &_container;
  std::unique_ptr<T> _task;
};

template <typename T> class ClearContainer : public Task {
public:
  ClearContainer(Container<std::unique_ptr<T>> &container)
      : _container(container) {}

  bool has_result() const override { return false; }

  std::string toString() const override {
    return std::format("Container is cleared");
  }

  void execute() override { _container.clear(); }

private:
  Container<std::unique_ptr<T>> &_container;
};

template <typename T> class CounterWithResult : public Task {
public:
  CounterWithResult(Container<std::unique_ptr<T>> &container)
      : _container(container) {}

  bool has_result() const override { return true; }

  std::string toString() const override {
    return std::format("Count of tasks with result in container: {}",
                       _counter.value());
  }

  void execute() override {
    size_t counter = 0;
    for (const auto &item : _container) {
      if (item->has_result()) {
        counter++;
      }
    }

    _counter.emplace(counter);
  }

private:
  std::optional<size_t> _counter;
  Container<std::unique_ptr<T>> &_container;
};

class ObjectCounter : public Task {
public:
  ObjectCounter() {}

  bool has_result() const override { return true; }

  std::string toString() const override {
    return std::format("Count of all objects: {}", _result.value());
  }

  void execute() override { _result = counter(); }

private:
  std::optional<size_t> _result;
};

template <typename T> class ObjectCounterContainer : public Task {
public:
  ObjectCounterContainer(Container<std::unique_ptr<T>> &container)
      : _container(container) {}

  bool has_result() const override { return true; }

  std::string toString() const override {
    return std::format("Count of objects in container: {}", _result.value());
  }

  void execute() override { _result = _container.size(); }

private:
  std::optional<size_t> _result;
  Container<std::unique_ptr<T>> _container;
};