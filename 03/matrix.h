class Matrix
{
    class MatrixProxy {
    public:
        MatrixProxy(size_t row, const Matrix* outer) : row(row), outer(outer) {}

        const int operator[](const size_t column) const {
            if (column >= outer->columns)
                throw std::out_of_range("");
            return outer->matrix[row][column];
        }

        int& operator[](const size_t column) {
            if (column >= outer->columns)
                throw std::out_of_range("");
            return outer->matrix[row][column];
        }

    private:
        size_t row;
        const Matrix* outer;
    };

public:
    Matrix(const size_t rows, const size_t columns) : rows(rows),
                                                      columns(columns) {
        matrix = new int*[rows];
        for (size_t i = 0; i < rows; i++)
            matrix[i] = new int[columns];
    }

    const size_t getRows() const {
        return rows;
    }

    const size_t getColumns() const {
        return columns;
    }

    const MatrixProxy operator[](const size_t row) const {
        if (row >= rows)
            throw std::out_of_range("");
        return (MatrixProxy(row, this));
    }

    MatrixProxy operator[](const size_t row) {
        if (row >= rows)
            throw std::out_of_range("");
        return (MatrixProxy(row, this));
    }

    Matrix& operator*=(const int var) {
        for (size_t i = 0; i < rows; i++)
            for (size_t j = 0; j < columns; j++)
                matrix[i][j] *= var;
        return *this;
    }

    bool operator==(const Matrix& matrix) const {
        if (columns != matrix.getColumns())
            return false;
        if (rows != matrix.getRows())
            return false;
        for (size_t i = 0; i < rows; i++)
            for (size_t j = 0; j < columns; j++)
                if (this->matrix[i][j] != matrix[i][j])
                    return false;
        return true;
    }

    bool operator!=(const Matrix& matrix) const {
        return !(*this == matrix);
    }

    virtual ~Matrix() {
        for (size_t i = 0; i < rows; i++)
            delete[] matrix[i];
        delete[] matrix;
    }

private:
    int** matrix = nullptr;
    const size_t rows;
    const size_t columns;
};
