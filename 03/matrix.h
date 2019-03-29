class Matrix
{
    class MatrixProxy {
    public:
        MatrixProxy(int* row, const Matrix& outer) : row(row), outer(outer) {}

        const int& operator[](const size_t column) const {
            if (column >= outer.columns)
                throw std::out_of_range("");
            return this->row[column];
        }

        int& operator[](const size_t column) {
            if (column >= outer.columns)
                throw std::out_of_range("");
            return this->row[column];
        }

    private:
        int* row = nullptr;
        const Matrix& outer;
    };

public:
    Matrix(const size_t rows, const size_t columns) : rows(rows),
                                                      columns(columns) {
        this->matrix = new int*[rows];
        for (size_t i = 0; i < this->rows; i++)
            matrix[i] = new int[columns];
    }

    const size_t getRows() const {
        return rows;
    }

    const size_t getColumns() const {
        return columns;
    }

    const MatrixProxy operator[](const size_t row) const {
        if (row >= this->rows)
            throw std::out_of_range("");
        return (MatrixProxy(this->matrix[row], *this));
    }

    MatrixProxy operator[](const size_t row) {
        if (row >= this->rows)
            throw std::out_of_range("");
        return (MatrixProxy(this->matrix[row], *this));
    }

    Matrix operator*=(const int var) {
        for (size_t i = 0; i < this->rows; i++)
            for (size_t j = 0; j < this->columns; j++)
                this->matrix[i][j] *= var;
        return *this;
    }

    bool operator==(const Matrix& matrix) const {
        if (this->columns != matrix.getColumns())
            return false;
        if (this->rows != matrix.getRows())
            return false;
        for (size_t i = 0; i < this->rows; i++)
            for (size_t j = 0; j < this->columns; j++)
                if (this->matrix[i][j] != matrix[i][j])
                    return false;
        return true;
    }

    bool operator!=(const Matrix& matrix) const {
        return !(*this == matrix);
    }

private:
    int** matrix = nullptr;
    const size_t rows = 0;
    const size_t columns = 0;
};
