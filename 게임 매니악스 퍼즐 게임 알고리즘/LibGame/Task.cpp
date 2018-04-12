#include "Task.h"
#include <stdio.h>
#include <assert.h>


//=============================================================
// Task 리스트 초기화
CTaskList::CTaskList(int max_task_size, int max_num_task)
:	MaxTaskSize(max_task_size), MaxNumTask(max_num_task), 
	NumFreeTask(max_num_task)
{
	// Task 초기화용 매크로
	#define TASK(INDEX) ((CTask*)(buf+max_task_size*(INDEX)))

	// Task용 메모리 확보
	char* buf=new char[max_task_size*(max_num_task+2)];

	// Active task 리스트의 초기화
	ActiveTask=TASK(0);
	ActiveTask->Prev=ActiveTask->Next=ActiveTask;

	// Free task 리스트의 초기화
	FreeTask=TASK(1);
	for (int i=1; i<max_num_task+1; i++) {
		TASK(i)->Next=TASK(i+1);
	}
	TASK(max_num_task+1)->Next=FreeTask;
}


//=============================================================
// Task 생성
// Free task 리스트 상에서는 메모리 블록의 앞부분에 CTask를 배치한다
// 서브 클래스의 생성자를 호출한 후에는
// 메모리 블록의 앞부분에 CTask가 배치되는 것은 아니다.
// 생성처리는 new와 생성자로 나누고,
// 삭제처리 역시 소멸자와 delete로 나눈다.

// new의 처리
void* CTask::operator_new(size_t t, CTaskList* task_list) {

	// 클래스 크기가 최대 크기를 넘으면 에러.
	assert(t<=(size_t)task_list->MaxTaskSize);

	// Free task 가 없을 때에는 NULL을 반환
	if (task_list->NumFreeTask<=0) return NULL;

	// Free task를 하나 꺼낸다
	CTask* task=task_list->FreeTask->Next;
	task_list->FreeTask->Next=task->Next;
	task_list->NumFreeTask--;
	
	// 생성자로
	return task;
}

// 생성자
CTask::CTask(CTaskList* task_list)
:	TaskList(task_list)
{

	// Task를 Active task 리스트에 넣는다.
	Prev=task_list->ActiveTask->Prev;
	Next=task_list->ActiveTask;
	Prev->Next=Next->Prev=this;
}


//=============================================================
// Task 삭제

// 소멸자
CTask::~CTask() {

	// Task를 Active task 리스트에서 삭제한다
	Prev->Next=Next;
	Next->Prev=Prev;
}

// delete의 처리
void CTask::operator_delete(void* p, CTaskList* task_list) {

	// Task
	CTask* task=(CTask*)p;

	// Task를 Free task 리스트에 넣는다.
	task->Next=task_list->FreeTask->Next;
	task_list->FreeTask->Next=task;
	task_list->NumFreeTask++;

	// Free task 수가 최대 task 수를 넘으면 에러.
	// (delete의 중복호출 등)
	assert(task_list->NumFreeTask<=task_list->MaxNumTask);
}


//=============================================================
// 모든 Task를 삭제
void CTaskList::DeleteTask() {
	for (CTaskIter i(this); i.HasNext(); i.Next(), i.Remove()) ;
}
