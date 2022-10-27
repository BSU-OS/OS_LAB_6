//
// Created by user on 19/10/2022.
//

#ifndef LAB5_BUFFEREDCHANNEL_H
#define LAB5_BUFFEREDCHANNEL_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>

template<class T>
class BufferedChannel {
public:
    explicit BufferedChannel(int size) : SIZE(size), closed(false) {}

    void Send(T value) {
        if (closed)
            throw std::runtime_error("");
        std::unique_lock<std::mutex> locker(lockSend);
        readyToSend.wait(locker, [&]() { return channelQueue.size() < SIZE; });
        channelQueue.push(value);
        readyToSend.notify_one();
    }

    std::pair<T, bool> Recv() {
        std::unique_lock<std::mutex> locker(lockRecv);
        if (closed) {
            if (channelQueue.empty())
                return std::make_pair(T(), false);
            else {
                std::pair<T, bool> answer = std::make_pair(channelQueue.front(), true);
                channelQueue.pop();
                return answer;
            }
        }
        readyToRecv.wait(locker, [&]() { return !channelQueue.empty(); });
        std::pair<T, bool> answer = std::make_pair(channelQueue.front(), true);
        channelQueue.pop();
        readyToRecv.notify_one();
        return answer;
    }

    void Close() {
        closed = true;
    }

private:
    bool closed;
    const int SIZE;
    std::queue<T> channelQueue;
    std::mutex lockSend;
    std::mutex lockRecv;
    std::condition_variable readyToSend;
    std::condition_variable readyToRecv;

};

#endif //LAB5_BUFFEREDCHANNEL_H
