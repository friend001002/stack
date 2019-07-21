#include <iostream>
#include <string>

using namespace std;

class Double_linked_list_ex : public runtime_error
{
  public:

  explicit Double_linked_list_ex(const char *msg) : runtime_error(msg)
  {
  }

  explicit Double_linked_list_ex(string& msg) : runtime_error(msg)
  {
  }

  virtual ~Double_linked_list_ex()
  {
  }

  virtual const char* what() const noexcept
  {
    return runtime_error::what();
  }
};

template<class val_type>
class Double_linked_node
{
  public:

  Double_linked_node() : val_{}, prev_{ nullptr }, next_{ nullptr }
  {}

  Double_linked_node(val_type val) : val_{ val }, prev_{ nullptr }, next_{ nullptr }
  {}

  Double_linked_node(const Double_linked_node<val_type>&) = delete;
  Double_linked_node(Double_linked_node<val_type>&&) = delete;

  Double_linked_node& operator=(const Double_linked_node<val_type>&) = delete;
  Double_linked_node& operator=(Double_linked_node<val_type>&&) = delete;

  virtual ~Double_linked_node()
  {
    cout << this << " double linked node (val = " << val_ << ", prev = " << prev_ << ", next = " << next_ << ") destr\n";

    val_ = {};
    next_ = { nullptr };
    prev_ = { nullptr };
  }

  val_type Get_val()
  {
    return val_;
  }

  Double_linked_node<val_type>* Get_prev()
  {
    return prev_;
  }

  Double_linked_node<val_type>* Get_next()
  {
    return next_;
  }

  template<class val_type> friend class Double_linked_list;

  private:

  val_type val_;

  Double_linked_node<val_type> *prev_;
  Double_linked_node<val_type> *next_;
};

template<class val_type>
class Double_linked_list
{
  public:

  Double_linked_list() : first_{ nullptr }, size_{}
  {}

  virtual ~Double_linked_list()
  {
    while (nullptr != first_)
    {
      void *tmp{ first_->next_ };

      delete first_;

      first_ = { static_cast<Double_linked_node<val_type>*>(tmp) };

      size_--;
    }

    if (0 != size_)
    {
      cerr << "~Double_linked_list: all elements were destroyed, but size = " << size_ << " (" << __FILE__ << ", " << __LINE__ << ")\n";
    }

    cout << this << " double linked list destr\n";
  }

  Double_linked_node<val_type>* Get_first()
  {
    return first_;
  }

  bool Emplace_element(val_type val, Double_linked_node<val_type> *after, Double_linked_node<val_type> **result)
  {
    if (nullptr == result)
    {
      cerr << "Emplace_element: Result node wasn't provided (" << __FILE__ << ' ' << __LINE__ << ")\n";
      return false;
    }

    // There were no nodes before
    if (nullptr == first_)
    {
      first_ = { new Double_linked_node<val_type>(val) };

      *result = { first_ };
      size_++;
      return true;
    }

    // Add as a first node
    if (nullptr == after)
    {
      *result = { new Double_linked_node<val_type>(val) };

      if (nullptr == *result)
      {
        cerr << "Emplace_element: Failed to create result node (" << __FILE__ << ' ' << __LINE__ << ")\n";
        return false;
      }

      Double_linked_node<val_type> *tmp{ first_ };

      first_ = { *result };

      (*result)->next_ = { tmp };
      tmp->prev_ = (*result);

      size_++;

      return true;
    }

    if (false == Find_node(after))
    {
      cerr << "Emplace_element: Failed to find node " << after << " in single linked list " << this << " (" << __FILE__ << ' ' << __LINE__ << ")\n";
      return false;
    }

    *result = { new Double_linked_node<val_type>(val) };

    if (nullptr == *result)
    {
      cerr << "Emplace_element: Failed to create result node (" << __FILE__ << ' ' << __LINE__ << ")\n";
      return false;
    }

    Double_linked_node<val_type> *tmp{ after->next_ };

    after->next_ = { *result };

    (*result)->next_ = { tmp };
    (*result)->prev_ = { after };

    if (nullptr != tmp)
    {
      tmp->prev_ = { *result };
    }
    size_++;

    return true;
  }

  Double_linked_node<val_type>* Emplace_element(val_type val, Double_linked_node<val_type> *after) noexcept(false)
  {
    Double_linked_node<val_type> *res{ nullptr };

    // There were no nodes before
    if (nullptr == first_)
    {
      first_ = { new Double_linked_node<val_type>(val) };

      size_++;
      return first_;
    }

    // Add as a first node
    if (nullptr == after)
    {
      res = { new Double_linked_node<val_type>(val) };

      if (nullptr == res)
      {
        cerr << "Emplace_element: Failed to create result node (" << __FILE__ << ' ' << __LINE__ << ")\n";

        throw Double_linked_list_ex("Failed to create result node\n");
      }

      Double_linked_node<val_type> *tmp{ first_ };

      first_ = { res };

      res->next_ = { tmp };
      tmp->prev_ = res;

      size_++;

      return res;
    }

    if (false == Find_node(after))
    {
      cerr << "Emplace_element: Failed to find node " << after << " in single linked list " << this << " (" << __FILE__ << ' ' << __LINE__ << ")\n";

      throw Double_linked_list_ex("Failed to find node\n");
    }

    res = { new Double_linked_node<val_type>(val) };

    if (nullptr == res)
    {
      cerr << "Emplace_element: Failed to create result node (" << __FILE__ << ' ' << __LINE__ << ")\n";

      throw Double_linked_list_ex("Failed to create result node\n");
    }

    Double_linked_node<val_type> *tmp{ after->next_ };

    after->next_ = { res };

    res->next_ = { tmp };
    res->prev_ = { after };

    if (nullptr != tmp)
    {
      tmp->prev_ = { res };
    }
    size_++;

    return res;
  }

  bool Add_element(Double_linked_node<val_type> **el, Double_linked_node<val_type> *after)
  {
    if (nullptr == el || nullptr == *el)
    {
      cerr << "Add_element: Element to add wasn't provided (" << __FILE__ << ' ' << __LINE__ << ")\n";
      return false;
    }

    if (nullptr != after && false == Find_node(after))
    {
      cerr << "Add_element: Failed to find node " << after << " in double linked list " << this << " (" << __FILE__ << ' ' << __LINE__ << ")\n";
      return false;
    }

    // There were no nodes before
    if (nullptr == first_)
    {
      first_ = { *el };
      size_++;

      return true;
    }

    // Add as a first node
    if (nullptr == after)
    {
      Double_linked_node<val_type> *tmp{ first_ };

      first_ = { *el };

      (*el)->next_ = { tmp };
      tmp->prev_ = (*el);

      size_++;

      return true;
    }

    Double_linked_node<val_type> *tmp{ after->next_ };

    after->next_ = { *el };

    (*el)->next_ = { tmp };
    (*el)->prev_ = { after };

    if (nullptr != tmp)
    {
      tmp->prev_ = { *el };
    }
    size_++;

    return true;
  }

  Double_linked_node<val_type>* Add_element(Double_linked_node<val_type> *el, Double_linked_node<val_type> *after) noexcept(false)
  {
    if (nullptr == el)
    {
      cerr << "Add_element: el must not be nullptr in double linked list " << this << " (" << __FILE__ << ' ' << __LINE__ << ")\n";

      throw Double_linked_list_ex("el must not be nullptr");
    }

    if (nullptr != after && false == Find_node(after))
    {
      cerr << "Add_element: Failed to find node " << after << " in double linked list " << this << " (" << __FILE__ << ' ' << __LINE__ << ")\n";

      throw Double_linked_list_ex("Failed to find node");
    }

    // There were no nodes before
    if (nullptr == first_)
    {
      first_ = { el };
      size_++;

      return first_;
    }

    // Add as a first node
    if (nullptr == after)
    {
      Double_linked_node<val_type> *tmp{ first_ };

      first_ = { el };

      el->next_ = { tmp };
      tmp->prev_ = { el };

      size_++;

      return first_;
    }

    Double_linked_node<val_type> *tmp{ after->next_ };

    after->next_ = { el };

    el->next_ = { tmp };
    el->prev_ = { after };

    if (nullptr != tmp)
    {
      tmp->prev_ = { el };
    }
    size_++;

    return after->next_;
  }

  bool Delete_element_cs(Double_linked_node<val_type> *el)
  {
    if (nullptr == el)
    {
      cerr << "Delete_element_cs: Node to delete wasn't provided (" << __FILE__ << ' ' << __LINE__ << ")\n";

      return false;
    }

    if (false == Find_node(el))
    {
      cerr << "Delete_element_cs: Failed to find node " << el << " in single linked list " << this << " (" << __FILE__ << ' ' << __LINE__ << ")\n";

      return false;
    }

    if (el == first_)
    {
      first_ = { el->next_ };
      el->~Double_linked_node();
      size_--;

      return true;
    }

    Double_linked_node<val_type> *parent{ el->prev_ };

    if (nullptr == parent)
    {
      cerr << "Delete_element_cs: node " << el << " isn't the first node, but doesn't have parents (" << __FILE__ << ' ' << __LINE__ << ")\n";
      return false;
    }

    if (nullptr == el->next_)
    {
      // No need to connect the el's child with a new parent

      parent->next_ = { nullptr };
      el->~Double_linked_node();
      size_--;

      return true;
    }
    else
    {
      // el's child will need a new parent -- el's parent

      parent->next_ = { el->next_ };
      el->~Double_linked_node();
      size_--;

      return true;
    }

    // Unreacheable for now
    //cout << "Delete_element: Failed to delete node " << el << " from single linked list " << this << " (" << __FILE__ << ' ' << __LINE__ << ")\n";
    //return false;
  }

  void Delete_element_ex(Double_linked_node<val_type> *el) noexcept(false)
  {
    if (nullptr == el)
    {
      cerr << "Delete_element_ex: Node to delete wasn't provided (" << __FILE__ << ' ' << __LINE__ << ")\n";

      throw Double_linked_list_ex("Node to delete wasn't provided");
    }

    if (false == Find_node(el))
    {
      cerr << "Delete_element_ex: Failed to find node " << el << " in single linked list " << this << " (" << __FILE__ << ' ' << __LINE__ << ")\n";

      throw Double_linked_list_ex("Failed to find node in single linked list");
    }

    if (el == first_)
    {
      first_ = { el->next_ };
      el->~Double_linked_node();
      size_--;

      return;
    }

    Double_linked_node<val_type> *parent{ el->prev_ };

    if (nullptr == parent)
    {
      cerr << "Delete_element_ex: node " << el << " isn't the first node, but doesn't have parents (" << __FILE__ << ' ' << __LINE__ << ")\n";

      throw Double_linked_list_ex("Parent is nullptr");
    }

    if (nullptr == el->next_)
    {
      // No need to connect the el's child with a new parent

      parent->next_ = { nullptr };
      el->~Double_linked_node();
      size_--;

      return;
    }
    else
    {
      // el's child will need a new parent -- el's parent

      parent->next_ = { el->next_ };
      el->~Double_linked_node();
      size_--;

      return;
    }

    // Unreacheable for now
    //cout << "Delete_element: Failed to delete node " << el << " from single linked list " << this << " (" << __FILE__ << ' ' << __LINE__ << ")\n";
    //return false;
  }

  bool Find_value_first_occur(val_type val, Double_linked_node<val_type> **result)
  {
    if (nullptr == result || nullptr == *result)
    {
      cerr << "Find_value_first_occur: Result node wasn't provided (" << __FILE__ << ' ' << __LINE__ << ")\n";

      return false;
    }

    if (nullptr == first_)
    {
      cerr << "Find_value_first_occur: Double linked list " << this << "doesn't have any nodes (" << __FILE__ << ' ' << __LINE__ << ")\n";

      return false;
    }

    Double_linked_node<val_type> *curr{ first_ };

    while (nullptr != curr)
    {
      if (curr->val_ == val)
      {
        *result = { curr };
        return true;
      }

      curr = curr->next_;
    }

    *result = { nullptr };
    cout << "Find_value_first_occur: Failed to find node vith val " << val << " in double linked list " << this << " (" << __FILE__ << ' ' << __LINE__ << ")\n";
    return false;
  }

  Double_linked_node<val_type>* Find_value_first_occur(val_type val) noexcept(false)
  {
    if (nullptr == first_)
    {
      cerr << "Find_value_first_occur: Double linked list " << this << "doesn't have any nodes (" << __FILE__ << ' ' << __LINE__ << ")\n";

      throw Double_linked_list_ex("Double linked list doesn't have any nodes");
    }

    Double_linked_node<val_type> *curr{ first_ };

    while (nullptr != curr)
    {
      if (curr->val_ == val)
      {
        return curr;
      }

      curr = curr->next_;
    }

    throw Double_linked_list_ex("Failed to find node in double linked list");
  }

  bool Find_node(Double_linked_node<val_type> *node)
  {
    if (nullptr == node)
    {
      cerr << "Find_node: Node to find wasn't provided (" << __FILE__ << ' ' << __LINE__ << ")\n";

      return false;
    }

    if (nullptr == first_)
    {
      cerr << "Find_node: Single linked list " << this << "doesn't have any nodes (" << __FILE__ << ' ' << __LINE__ << ")\n";

      return false;
    }

    Double_linked_node<val_type> *curr{ first_ };

    while (nullptr != curr)
    {
      if (curr == node)
      {
        return true;
      }

      curr = { curr->next_ };
    }

    cout << "Find_node: Node " << node << " is not a part of linked list " << this << " (" << __FILE__ << ' ' << __LINE__ << ")\n";
    return false;
  }

  Double_linked_node<val_type>* operator[](size_t index) const
  {
    if (nullptr == first_)
    {
      cerr << "Double_linked_node operator[]: no elements (" << __FILE__ << ", " << __LINE__ << ")\n";

      throw Double_linked_list_ex("Double linked list has no elements");
    }

    if (index >= size_)
    {
      cerr << "Double_linked_node operator[]: index " << index << " is out of bounds (" << __FILE__ << ", " << __LINE__ << ")\n";

      throw Double_linked_list_ex("Double linked list: index is out of bounds");
    }

    Double_linked_node<val_type> *curr{ first_ };
    val_type ret{};

    for (size_t i{}; i < index; ++i)
    {
      if (nullptr != curr)
      {
        curr = { curr->next_ };
      }
      else
      {
        cerr << "Double_linked_node operator[]: Can't access element with index" << index << " (" << __FILE__ << ", " << __LINE__ << ")\n";

        throw Double_linked_list_ex("Can't access element with this index");
      }
    }

    return curr;
  }

  size_t Get_size() const noexcept
  {
    return size_;
  }

  bool Move_after_cs(size_t what, size_t after)
  {
    if (what >= size_)
    {
      cerr << "Double_linked_node Move_after_cs: what (" << what << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (after >= size_)
    {
      cerr << "Double_linked_node Move_after_cs: after (" << after << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (what == after)
    {
      return true;
    }

    if (abs(what - after) == 1 && what > after)
    {
      return true;
    }

    Double_linked_node<val_type> *what_ptr{ operator[](what) };
    Double_linked_node<val_type> *after_ptr{ operator[](after) };

    return Move_cs(what_ptr, after_ptr);
  }

  void Move_after_ex(size_t what, size_t after) noexcept(false)
  {
    if (what >= size_)
    {
      cerr << "Double_linked_node Move_after_ex: what (" << what << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";

      throw Double_linked_list_ex("Move_after_ex: 'what' must be < size");
    }

    if (after >= size_)
    {
      cerr << "Double_linked_node Move_after_ex: after (" << after << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";

      throw Double_linked_list_ex("Move_after_ex: 'after' must be < size");
    }

    if (what == after)
    {
      return;
    }

    if (abs(what - after) == 1 && what > after)
    {
      return;
    }

    Double_linked_node<val_type> *what_ptr{ operator[](what) };
    Double_linked_node<val_type> *after_ptr{ operator[](after) };

    return Move_ex(what_ptr, after_ptr);
  }

  bool Move_before_cs(size_t what, size_t before)
  {
    if (what >= size_)
    {
      cerr << "Double_linked_node Move_before_cs: what (" << what << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (before >= size_)
    {
      cerr << "Double_linked_node Move_before_cs: before (" << before << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (what == before)
    {
      return true;
    }

    if (abs(what - before) == 1 && what < before)
    {
      return true;
    }

    Double_linked_node<val_type> *what_ptr{ operator[](what) };

    if (before > 0)
    {
      Double_linked_node<val_type> *after_ptr{ operator[](before - 1) };

      return Move_cs(what_ptr, after_ptr);
    }
    else
    {
      return Move_cs(what_ptr, nullptr);
    }
  }

  void Move_before_ex(size_t what, size_t before) noexcept(false)
  {
    if (what >= size_)
    {
      cerr << "Double_linked_node Move_before_ex: what (" << what << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";

      throw Double_linked_list_ex("Move_before_ex: 'what' must be < size");
    }

    if (before >= size_)
    {
      cerr << "Double_linked_node Move_before_ex: before (" << before << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";

      throw Double_linked_list_ex("Move_before_ex: 'before' must be < size");
    }

    if (what == before)
    {
      return;
    }

    if (abs(what - before) == 1 && what < before)
    {
      return;
    }

    Double_linked_node<val_type> *what_ptr{ operator[](what) };

    if (before > 0)
    {
      Double_linked_node<val_type> *after_ptr{ operator[](before - 1) };

      return Move_cs(what_ptr, after_ptr);
    }
    else
    {
      return Move_cs(what_ptr, nullptr);
    }
  }

  bool Swap_vals_cs(size_t a, size_t b)
  {
    if (a >= size_)
    {
      cerr << "Double_linked_node Swap_vals_cs: a (" << a << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (b >= size_)
    {
      cerr << "Double_linked_node Swap_vals_cs: b (" << b << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (a == b)
    {
      return true;
    }

    Double_linked_node<val_type> *a_ptr{ operator[](a) };
    Double_linked_node<val_type> *b_ptr{ operator[](b) };

    return Swap_vals_cs(a_ptr, b_ptr);
  }

  void Swap_vals_ex(size_t a, size_t b)
  {
    if (a >= size_)
    {
      cerr << "Double_linked_node Swap_vals_ex: a (" << a << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";

      throw Double_linked_list_ex("Swap_vals_ex: 'a' must be < size");
    }

    if (b >= size_)
    {
      cerr << "Double_linked_node Swap_vals_ex: b (" << b << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";

      throw Double_linked_list_ex("Swap_vals_ex: 'b' must be < size");
    }

    if (a == b)
    {
      return;
    }

    Double_linked_node<val_type> *a_ptr{ operator[](a) };
    Double_linked_node<val_type> *b_ptr{ operator[](b) };

    return Swap_vals_ex(a_ptr, b_ptr);
  }

  bool Swap_vals_cs(Double_linked_node<val_type>* a, Double_linked_node<val_type>* b)
  {
    if (nullptr == a)
    {
      cerr << "Double_linked_node Swap_vals_cs: a (" << a << ") must not be nullptr (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (nullptr == b)
    {
      cerr << "Double_linked_node Swap_vals_cs: b (" << b << ") must not be nullptr (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (a == b)
    {
      return true;
    }

    if (false == Find_node(a))
    {
      cerr << "Double_linked_node Swap_vals_cs: node a (" << a << ") must be a part of linked list (" << this << ") (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (false == Find_node(b))
    {
      cerr << "Double_linked_node Swap_vals_cs: node b (" << b << ") must be a part of linked list (" << this << ") (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    val_type tmp{ a->val_ };
    a->val_ = { b->val_ };
    b->val_ = { tmp };

    return true;
  }

  void Swap_vals_ex(Double_linked_node<val_type>* a, Double_linked_node<val_type>* b)
  {
    if (nullptr == a)
    {
      cerr << "Double_linked_node Swap_vals_ex: a (" << a << ") must not be nullptr (" << __FILE__ << ", " << __LINE__ << ")\n";

      throw Double_linked_list_ex("Swap_vals_ex: 'a' must not be nullptr");
    }

    if (nullptr == b)
    {
      cerr << "Double_linked_node Swap_vals_ex: b (" << b << ") must not be nullptr (" << __FILE__ << ", " << __LINE__ << ")\n";

      throw Double_linked_list_ex("Swap_vals_ex: 'b' must not be nullptr");
    }

    if (a == b)
    {
      return;
    }

    if (false == Find_node(a))
    {
      cerr << "Double_linked_node Swap_vals_ex: node a (" << a << ") must be a part of linked list (" << this << ") (" << __FILE__ << ", " << __LINE__ << ")\n";

      throw Double_linked_list_ex("Swap_vals_ex: 'a' must be a part of linked list");
    }

    if (false == Find_node(b))
    {
      cerr << "Double_linked_node Swap_vals_ex: node b (" << b << ") must be a part of linked list (" << this << ") (" << __FILE__ << ", " << __LINE__ << ")\n";

      throw Double_linked_list_ex("Swap_vals_ex: 'b' must be a part of linked list");
    }

    val_type tmp{ a->val_ };
    a->val_ = { b->val_ };
    b->val_ = { tmp };

    return;
  }

private:

  bool Move_cs(Double_linked_node<val_type>* what, Double_linked_node<val_type>* after)
  {
    if (nullptr == what)
    {
      cerr << "Double_linked_node Move_cs: what (" << what << ") must not be nullptr (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (false == Find_node(what))
    {
      cerr << "Double_linked_node Move_cs: node what (" << what << ") must be a part of linked list (" << this << ") (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (what == after)
    {
      return true;
    }

    Double_linked_node<val_type> *what_prev_old{ what->prev_ };
    Double_linked_node<val_type> *what_next_old{ what->next_ };
    Double_linked_node<val_type> *after_next_old{};

    if (nullptr != after)
    {
      after_next_old = { after->next_ };
      after->next_ = { nullptr };
    }

    what->prev_ = what->next_ = { nullptr };

    if (nullptr != what_prev_old)
    {
      what_prev_old->next_ = { what_next_old };
    }

    if (nullptr != what_next_old)
    {
      what_next_old->prev_ = { what_prev_old };
    }

    if (nullptr != after)
    {
      after->next_ = { what };
    }

    what->prev_ = { after };
    if (nullptr != after_next_old)
    {
      what->next_ = { after_next_old };
    }
    else if (nullptr == after)
    {
      what->next_ = { first_ };
    }

    if (nullptr != after_next_old)
    {
      after_next_old->prev_ = { what };
    }

    if (what == first_)
    {
      first_ = { what_next_old };
    }

    if (nullptr == after)
    {
      first_->prev_ = { what };
      first_ = { what };
    }

    return true;
  }

  void Move_ex(Double_linked_node<val_type>* what, Double_linked_node<val_type>* after)
  {
    if (nullptr == what)
    {
      cerr << "Double_linked_node Move: what (" << what << ") must not be nullptr (" << __FILE__ << ", " << __LINE__ << ")\n";

      throw Double_linked_list_ex("Move_ex: 'what' must not be nullptr");
    }

    if (false == Find_node(what))
    {
      cerr << "Double_linked_node Move: node what (" << what << ") must be a part of linked list (" << this << ") (" << __FILE__ << ", " << __LINE__ << ")\n";

      throw Double_linked_list_ex("Move_ex: 'what' must be a part of linked list");
    }

    if (what == after)
    {
      return;
    }

    Double_linked_node<val_type> *what_prev_old{ what->prev_ };
    Double_linked_node<val_type> *what_next_old{ what->next_ };
    Double_linked_node<val_type> *after_next_old{};

    if (nullptr != after)
    {
      after_next_old = { after->next_ };
      after->next_ = { nullptr };
    }

    what->prev_ = what->next_ = { nullptr };

    if (nullptr != what_prev_old)
    {
      what_prev_old->next_ = { what_next_old };
    }

    if (nullptr != what_next_old)
    {
      what_next_old->prev_ = { what_prev_old };
    }

    if (nullptr != after)
    {
      after->next_ = { what };
    }

    what->prev_ = { after };
    if (nullptr != after_next_old)
    {
      what->next_ = { after_next_old };
    }
    else if (nullptr == after)
    {
      what->next_ = { first_ };
    }

    if (nullptr != after_next_old)
    {
      after_next_old->prev_ = { what };
    }

    if (what == first_)
    {
      first_ = { what_next_old };
    }

    if (nullptr == after)
    {
      first_->prev_ = { what };
      first_ = { what };
    }

    return;
  }

  size_t size_;
  Double_linked_node<val_type> *first_;
};

class Stack_ex : public Double_linked_list_ex
{
  public:

  explicit Stack_ex(const char *msg) : Double_linked_list_ex(msg)
  {}

  explicit Stack_ex(string& msg) : Double_linked_list_ex(msg)
  {}

  virtual ~Stack_ex()
  {}

  virtual const char* what() const noexcept
  {
    return Double_linked_list_ex::what();
  }
};

template<class val_type>
class Stack
{
  public:

  Stack() : data_{new Double_linked_list<val_type>}
  {}

  virtual ~Stack()
  {
    cout << "~Stack\n";

    delete data_;
  }

  val_type Pop() noexcept(false)
  {
    size_t size{ data_->Get_size() };

    if (0 == size)
    {
      throw Stack_ex("Stack Pop: Stack is empty.");
    }

    Double_linked_node<val_type> *node { data_->operator[](size - 1) };

    if (nullptr == node)
    {
      throw Stack_ex("Stack Pop: Failed to acces stack's top.");
    }

    val_type ret { node->Get_val() };

    try
    {
      data_->Delete_element_ex(node);
    }
    catch (Double_linked_list_ex& ex)
    {
      throw Stack_ex("Stack Pop: Failed to delete stack's top.");
    }

    return ret;
  }

  val_type Peek() noexcept(false)
  {
    size_t size { data_->Get_size() };

    if (0 == size)
    {
      throw Stack_ex("Stack Peek: Stack is empty.");
    }

    Double_linked_node<val_type> *node { data_->operator[](size - 1) };

    if (nullptr == node)
    {
      throw Stack_ex("Stack Peek: Failed to acces stack's top.");
    }

    return node->Get_val();
  }

  void Push(val_type val) noexcept(false)
  {
    Double_linked_node<val_type> *node { new Double_linked_node<val_type>(val) };

    if (nullptr == node)
    {
      throw Stack_ex("Stack Push: Failed to create a new member of the stack.");
    }

    if (0 == data_->Get_size())
    {
      try
      {
        node = { data_->Add_element(node, nullptr) };
      }
      catch (Double_linked_list_ex& ex)
      {
        throw Stack_ex("Stack Push: Failed to add a new member to the stack.");
      }
    }
    else
    {
      Double_linked_node<val_type> *last_node { data_->operator[](data_->Get_size() - 1) };

      if (nullptr == last_node)
      {
        throw Stack_ex("Stack Push: Failed to acces stack's top!");
      }

      try
      {
        node = { data_->Add_element(node, last_node) };
      }
      catch (Double_linked_list_ex& ex)
      {
        throw Stack_ex("Stack Push: Failed to add a new member to the stack.");
      }
    }
  }

  size_t Get_size()
  {
    return data_->Get_size();
  }

  private:

  Double_linked_list<val_type> *data_;
};

int main()
{
  do
  {
    Stack<float> st;

    try
    {
      cout << st.Peek() << " size " << st.Get_size() << endl;
    }
    catch (Stack_ex& ex)
    {
      cout << ex.what() << endl;
    }

    st.Push(1.0);

    cout << st.Peek() << " size " << st.Get_size() << endl;

    st.Push(2.0);

    cout << st.Peek() << " size " << st.Get_size() << endl;

    cout << st.Pop() << " size " << st.Get_size() << endl;

    cout << st.Peek() << " size " << st.Get_size() << endl;

    cout << st.Pop() << " size " << st.Get_size() << endl;

    try
    {
      cout << st.Peek() << " size " << st.Get_size() << endl;

    }
    catch (Stack_ex& ex)
    {
      cout << ex.what() << endl;
    }

    try
    {
      cout << st.Pop() << " size " << st.Get_size() << endl;
    }
    catch (Stack_ex& ex)
    {
      cout << ex.what() << endl;
    }
  }
  while (false);

  cin.get();

  return 0;
}
