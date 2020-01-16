#include <iostream>
#include <future>
#include <functional>
#include <vector>
#include <memory>
#include <numeric>
#include <chrono>

//compilation command:
//g++ min_working_threading_example.cxx -pthread -std=c++11

//https://ask.xmodulo.com/view-threads-process-linux.html
// https://stackoverflow.com/questions/48857679/stdasync-with-class-member-function
// https://stackoverflow.com/questions/14912635/stdasync-call-of-member-function
// https://www.reddit.com/r/learnprogramming/comments/2gavlf/c11_what_is_the_proper_way_to_pass_a_member/
// https://stackoverflow.com/questions/11758414/class-and-stdasync-on-class-member-in-c/11758662
// https://stackoverflow.com/questions/46031200/using-stdfuture-stdasync
// https://stackoverflow.com/questions/58783369/using-stdasync-to-parallelize-loop-over-instances-of-a-class
//https://stackoverflow.com/questions/17274032/c-threads-stdsystem-error-operation-not-permitted

typedef std::chrono::time_point<std::chrono::high_resolution_clock> tchrono;

tchrono Get_Current_Time_chrono()
{
  return std::chrono::high_resolution_clock::now();
}

int Get_us_Elapsed_Since(tchrono t0)
{
  tchrono t1 = Get_Current_Time_chrono();
  int dt= std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();
  return dt;
}



class myClass
{
  public:
    void Init(long int& size)
    {
      vecsize=size;
    }

    void Set_time(int& t)
    {
      time=t;
    }

    void Calc_At_this_time()
    {
      for (int i=0;i<vecsize;++i) vec.push_back(i+time);
      for (int i=0;i<vecsize;++i) --vec[i];
      for (int i=0;i<vecsize;++i) --vec[i];
      for (int i=0;i<vecsize;++i) --vec[i];
      for (int i=0;i<vecsize;++i) ++vec[i];

      sum= std::accumulate(vec.begin(),vec.end(),0);
    }

    void Parallelize_Calc_At_this_time()
    {
      auto g=std::bind(&myClass::Calc_At_this_time, this);
      f= std::async(std::launch::async, g);
    }

    long int Get_Result()
    {
      return sum;
    }

    long int Parallelize_Get_Result()
    {
      if (f.valid()) f.wait();
      return sum;
    }

  private:
    std::vector<int> vec;
    long int sum;
    int time;
    long int vecsize;
    std::future<void> f;
};


int main()
{
  std::vector<std::unique_ptr<myClass>> ptrs_vec;

  for (long int i=1; i<6; ++i)
  {
    std::unique_ptr<myClass> temp_ptr(new myClass);
    ptrs_vec.push_back(std::move(temp_ptr));
    //initialize the added member.
    long int i2=i*1000;
    (ptrs_vec.back())->Init(i2);
  }

  tchrono t0=Get_Current_Time_chrono();
  for (int t=1;t<10;++t)
  {
    long int sum_results=0;

    // i want every call in this loop to run parallely
    for (int i=0; i<ptrs_vec.size(); ++i)
    {
      ptrs_vec[i]->Set_time(t);
      // ptrs_vec[i]->Calc_At_this_time();
      ptrs_vec[i]->Parallelize_Calc_At_this_time();
    }

    for (int i=0; i<ptrs_vec.size(); ++i)
    {
      // sum_results+= ptrs_vec[i]->Get_Result();
      sum_results+= ptrs_vec[i]->Parallelize_Get_Result();
    }

    std::cout << "t: " << t << " sum_of_sums: " <<  sum_results<< std::endl;
  }
  std::cout << "dt: "<< Get_us_Elapsed_Since(t0) << std::endl;


  t0=Get_Current_Time_chrono();
  for (int t=1;t<10;++t)
  {
    long int sum_results=0;

    for (int i=0; i<ptrs_vec.size(); ++i)
    {
      ptrs_vec[i]->Set_time(t);
      ptrs_vec[i]->Calc_At_this_time();
    }

    for (int i=0; i<ptrs_vec.size(); ++i)
    {
      sum_results+= ptrs_vec[i]->Get_Result();
    }

    std::cout << "t: " << t << " sum_of_sums: " <<  sum_results<< std::endl;
  }
  std::cout << "dt: "<< Get_us_Elapsed_Since(t0) << std::endl;

  return 0;
}
