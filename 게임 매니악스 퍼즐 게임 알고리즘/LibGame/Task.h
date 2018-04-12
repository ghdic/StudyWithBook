#ifndef TASKH
#define TASKH

class CTask;
class CTaskList;
class CTaskIter;


//==============================================================
// Task
class CTask {
friend CTaskList;
friend CTaskIter;

	// Task 리스트의 포인터
	CTaskList* TaskList;

	// 앞,뒤 Task의 포인터
	CTask *Prev, *Next;

	// Task 리스트를 지정하지 않은 new, delete를 무효화 한다.
	void* operator new(size_t t) {}
	void operator delete(void* p) {}

protected:
	
	// new, delete의 처리 :
	// 적절한 new, delete 연산자를 서브 클래스에서 정의한다.
	static void* operator_new(size_t t, CTaskList* task_list);
	static void operator_delete(void* p, CTaskList* task_list);

public:
	CTask(CTaskList* task_list);
	virtual ~CTask();
};


//==============================================================
// Task 리스트
class CTaskList {
friend CTask;
friend CTaskIter;

	// Active task 리스트, Free task 리스트
	CTask *ActiveTask, *FreeTask;

	// Task의 최대 크기, Task의 최대 수
	int MaxTaskSize, MaxNumTask;
	
	// Free task의 수
	int NumFreeTask;

public:

	// 생성자
	CTaskList(int max_task_size, int max_num_task);
	
	// Task의 수
	int GetNumFreeTask() { return NumFreeTask; }
	int GetNumActiveTask() { return MaxNumTask-NumFreeTask; }

	// 모든 Task를 삭제
	void DeleteTask();
};


//==============================================================
// Task iterator
class CTaskIter {
protected:
	CTaskList* TaskList;
	CTask* Task;
public:

	// 생성자
	inline CTaskIter(CTaskList* task_list)
	: TaskList(task_list), Task(task_list->ActiveTask)
	{}

	// 다음 요소가 있을 경우 true
	inline bool HasNext() {
		return Task->Next!=TaskList->ActiveTask;
	}

	// 다음 요소를 반환
	inline CTask* Next() {
		return Task=Task->Next;
	}

	// 직전에 반환한 요소를 삭제
	inline void Remove() {
		Task=Task->Prev;
		delete Task->Next;
	}
};


#endif

