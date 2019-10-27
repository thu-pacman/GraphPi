#pragma once

class Prefix
{
public:
    Prefix();
    ~Prefix();
    void init(int input_size, const int* input_data);
    bool operator ==(const Prefix& pre) const;
    bool operator !=(const Prefix& pre) const;
    bool equal(int input_size, const int* input_data) const;
    inline int get_size() const { return size;}
    inline const int* get_data_ptr() const { return data;}
    inline int get_data(int index) const { return data[index];}

private:
    int size;
    int* data;
};