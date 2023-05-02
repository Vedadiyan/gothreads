package main

import (
	"fmt"
	"sync"
	"sync/atomic"
	"time"
)

//#include "lib/threads.c"
import "C"

//export callback
func callback(id C.int) {
	fn, ok := _threadPool.Load(id)
	C.SetAttributes()
	if ok {
		fn.(func())()
		return
	}
	panic("thread id not found")
}

var _threadPool sync.Map
var _id atomic.Int32

type Thread struct {
	id     C.int
	done   bool
	result chan any
	handle C.handle
}

func New(fn func() any) *Thread {
	thread := Thread{
		id:     C.int(_id.Add(1)),
		done:   false,
		result: make(chan any),
	}
	_threadPool.Store(thread.id, func() {
		result := fn()
		C.Close(thread.handle)
		thread.done = true
		thread.result <- result
	})
	return &thread
}

func (t *Thread) Start() {
	t.handle = C.Create(C.int(t.id))
}

func (t *Thread) Stop() {
	C.Terminate(t.handle)
	_threadPool.Delete(t.id)
}

func (t *Thread) Suspend() {
	C.Suspend(t.handle)
}

func (t *Thread) Resume() {
	C.Resume(t.handle)
}

func (t *Thread) Wait() any {
	C.Join(t.handle)
	return t.result
}

func (t *Thread) WaitWithTimeout(time time.Duration) any {
	C.JoinWithTimeout(t.handle, C.DWORD(time.Milliseconds()))
	return t.result
}

func (t *Thread) Await() chan any {
	return t.result
}

func main() {
	t := New(func() any {
		for i := 0; i < 1000; i++ {
			fmt.Println(i)
			<-time.After(time.Second)
		}
		return nil
	})
	t.Start()
	<-time.After(time.Second * 5)
	t.Suspend()
	<-time.After(time.Second * 5)
	t.Resume()
	<-time.After(time.Second * 5)
	t.Suspend()
	<-time.After(time.Second * 5)
	t.Resume()
	<-time.After(time.Hour)
}
