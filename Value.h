#ifndef VALUE_H
#define VALUE_H

template <typename T>
class Value {
  public:
    T &val;

    Value(T &value, T d_val, T min, T max, T inc_dec_val):
      val(value),
      _min(min),
      _max(max),
      _idv(inc_dec_val) {
      val = d_val;
    }

    bool inc() {
      if (val + _idv > _max) return false;
      val += _idv;
      return true;
    }

    bool dec() {
      if (val - _idv < _min) return false;
      val -= _idv;
      return true;
    }

    byte change(int times) {
      byte times_abs = abs(times);
      byte res = 0;
      for (byte i = 0; i < times_abs; ++i) {
        if (times > 0 ? inc() : dec()) {
          res++;
          continue;
        }
        break;
      }
      return res;
    }
  private:
    const T _min;
    const T _max;
    const T _idv;
};

#endif
