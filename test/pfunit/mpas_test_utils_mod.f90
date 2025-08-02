module mpas_test_utils_mod
    public :: file_exists, delete_file
contains
    ! Helper: Check if a file exists
    logical function file_exists(path)
        character(len = *), intent(in) :: path
        logical :: exists
        inquire(file = path, exist = exists)
        file_exists = exists
    end function file_exists

    ! Helper: Delete file if it exists
    subroutine delete_file(path)
        character(len = *), intent(in) :: path
        if (file_exists(path)) then
            open(unit = 99, file = path, status = 'old')
            close(99, status = 'delete')
        end if
    end subroutine delete_file

    function extract_int_after_equals(line) result(value)
        character(len = *), intent(in) :: line
        integer :: value
        integer :: eq_pos, istart, iend
        character(len = 32) :: numstr

        eq_pos = index(line, "=")
        if (eq_pos == 0) then
            value = -1  ! or some default error value
            return
        end if
        istart = eq_pos + 1
        do while (istart <= len_trim(line) .and. line(istart:istart) == ' ')
            istart = istart + 1
        end do
        iend = istart
        do while (iend <= len_trim(line) .and. line(iend:iend) >= '0' .and. line(iend:iend) <= '9')
            iend = iend + 1
        end do
        if (iend > istart) then
            read(line(istart:iend - 1), *) value
        else
            value = -1
        end if
    end function extract_int_after_equals

end module mpas_test_utils_mod

