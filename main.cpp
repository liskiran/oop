#include <memory>

#include "Object.h"

enum TaskNumber {
  BinaryTask = 1,
  AddOperationTask = 2,
  ClearContainerTask = 3,
  CounterTask = 4,
  ObjectCounterTask = 5,
  ObjectCounterContainerTask = 6,
};

int random(int min, int max) { return min + rand() % ((max + 1) - min); }

const int MIN_TASK_RANGE = 1;
const int MAX_TASK_RANGE = 6;

const int MIN_NUMBER_RANGE = 1;
const int MAX_NUMBER_RANGE = 20;

std::unique_ptr<Task>
generate_task(Container<std::unique_ptr<Task>> &container) {
  const int task_number = random(MIN_TASK_RANGE, MAX_TASK_RANGE);

  switch (auto operation = static_cast<TaskNumber>(task_number); operation) {
  case BinaryTask: {
    auto random_first_value = random(MIN_NUMBER_RANGE, MAX_NUMBER_RANGE);
    auto random_second_value = random(MIN_NUMBER_RANGE, MAX_NUMBER_RANGE);

    switch (auto random_binary_operation = random(1, 4);
            random_binary_operation) {
    case 1: {
      return std::make_unique<Binary<int>>(
          Binary(Operation::Add, random_first_value, random_second_value));
      break;
    }
    case 2: {
      return std::make_unique<Binary<int>>(
          Binary(Operation::Subtract, random_first_value, random_second_value));
      break;
    }
    case 3: {
      return std::make_unique<Binary<int>>(
          Binary(Operation::Divide, random_first_value, random_second_value));
    }
    case 4: {
      return std::make_unique<Binary<int>>(
          Binary(Operation::Multiply, random_first_value, random_second_value));
      break;
    }
    }
  }

  case AddOperationTask: {
    return std::make_unique<Add<Task>>(std::move(generate_task(container)),
                                       container);
    break;
  }
  case ClearContainerTask: {
    return std::make_unique<ClearContainer<Task>>(ClearContainer(container));
    break;
  }
  case CounterTask: {
    return std::make_unique<CounterWithResult<Task>>(
        CounterWithResult(container));
    break;
  }
  case ObjectCounterTask: {
    return std::make_unique<ObjectCounter>(ObjectCounter());
    break;
  }
  case ObjectCounterContainerTask: {
    return std::make_unique<ObjectCounterContainer<Task>>(
        ObjectCounterContainer(container));
    break;
  }
  }
}

const int COUNT_TASK_GENERATING = 20;

int main() {
  srand(time(NULL));

  Container<std::unique_ptr<Task>> task_list;
  Container<std::string> task_informations_list;

  for (size_t i = 0; i < COUNT_TASK_GENERATING; i++) {
    task_list.push_back(generate_task(task_list));
  }

  std::cout << "Task list size: " << task_list.size() << '\n';

  for (auto &task : task_list) {
    task->execute();
    task_informations_list.push_back(task->toString());
    if (task_list.empty()) {
      break;
    }
  }

  std::cout << task_list.size() << '\n';

  for (const auto &task_info : task_informations_list) {
    std::cout << task_info << "\n";
  }

  if (!task_list.empty()) {
    task_list.clear();
  }

  task_informations_list.clear();

  std::cout << "task list size after clearing: " << task_list.size() << '\n';
  std::cout << "task informations size after cleaning: "
            << task_informations_list.size() << '\n';

  return 0;
}
