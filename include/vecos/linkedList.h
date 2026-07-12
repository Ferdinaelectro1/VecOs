#pragma once

namespace vecos {
    class ListNode {
      public:
        ListNode *next = nullptr;
    }; 

    class LinkedList {
        public:
          LinkedList();
          void push(ListNode* value);
          ListNode* read();
          bool empty() const;

        private:
          ListNode* _head;
          ListNode* _tail;

    };
};