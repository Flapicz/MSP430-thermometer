import {
  Stack,
  Box,
  TableContainer,
  Table,
  Paper,
  TableHead,
  TableRow,
  TableCell,
  TableBody,
  TablePagination,
} from "@mui/material";
import { useState, useEffect } from "react";
import * as React from "react";
import { useTheme } from "@mui/material/styles";
import PropTypes from "prop-types";
import axios from "axios";
import TableFooter from "@mui/material/TableFooter";
import IconButton from "@mui/material/IconButton";
import FirstPageIcon from "@mui/icons-material/FirstPage";
import KeyboardArrowLeft from "@mui/icons-material/KeyboardArrowLeft";
import KeyboardArrowRight from "@mui/icons-material/KeyboardArrowRight";
import LastPageIcon from "@mui/icons-material/LastPage";
import { ThemeProvider, createTheme } from "@mui/material/styles";

function TablePaginationActions(props) {
  const theme = useTheme();
  const { count, page, rowsPerPage, onPageChange } = props;

  const handleFirstPageButtonClick = (event) => {
    onPageChange(event, 0);
  };

  const handleBackButtonClick = (event) => {
    onPageChange(event, page - 1);
  };

  const handleNextButtonClick = (event) => {
    onPageChange(event, page + 1);
  };

  const handleLastPageButtonClick = (event) => {
    onPageChange(event, Math.max(0, Math.ceil(count / rowsPerPage) - 1));
  };

  return (
    <Box sx={{ flexShrink: 0, ml: 2.5 }}>
      <IconButton
        onClick={handleFirstPageButtonClick}
        disabled={page === 0}
        aria-label="first page"
      >
        {theme.direction === "rtl" ? <LastPageIcon /> : <FirstPageIcon />}
      </IconButton>
      <IconButton
        onClick={handleBackButtonClick}
        disabled={page === 0}
        aria-label="previous page"
      >
        {theme.direction === "rtl" ? (
          <KeyboardArrowRight />
        ) : (
          <KeyboardArrowLeft />
        )}
      </IconButton>
      <IconButton
        onClick={handleNextButtonClick}
        disabled={page >= Math.ceil(count / rowsPerPage) - 1}
        aria-label="next page"
      >
        {theme.direction === "rtl" ? (
          <KeyboardArrowLeft />
        ) : (
          <KeyboardArrowRight />
        )}
      </IconButton>
      <IconButton
        onClick={handleLastPageButtonClick}
        disabled={page >= Math.ceil(count / rowsPerPage) - 1}
        aria-label="last page"
      >
        {theme.direction === "rtl" ? <FirstPageIcon /> : <LastPageIcon />}
      </IconButton>
    </Box>
  );
}
TablePaginationActions.propTypes = {
  count: PropTypes.number.isRequired,
  onPageChange: PropTypes.func.isRequired,
  page: PropTypes.number.isRequired,
  rowsPerPage: PropTypes.number.isRequired,
};

function App() {
  const [data, setData] = useState([]);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);
  const [page, setPage] = React.useState(0);
  const [rowsPerPage, setRowsPerPage] = React.useState(12);
  const darkTheme = createTheme({
    typography:{
      fontFamily: ['Roboto','Helvetica'].join(','),
      fontSize: 16
    },
    palette: {
      mode: "dark",
    },
  });

  const roundTemp = (tempString) => {
    return parseFloat(tempString).toFixed(1);
  };

  const emptyRows =
    page > 0 ? Math.max(0, (1 + page) * rowsPerPage - data.length) : 0;

  const handleChangePage = (event, newPage) => {
    setPage(newPage);
  };

  useEffect(() => {
    async function fetchData() {
      try {
        setLoading(true);
        let res = await axios.get("http://frog01-20917.wykr.es/");
        let temperatures = res.data;
        setData(temperatures);
      } catch (error) {
        setLoading(false);
        setError("An error occurred. Awkward..");
      }
    }
    setInterval(()=>fetchData(), 1000)
  }, []);
  return (
    <ThemeProvider theme={darkTheme}>
      <Box sx={{display: 'flex', justifyContent: 'center'}}>
          <Stack sx={{width: '500px'}}direction={'column'} alignItems={'stretch'} justifyContent={'center'} >
          <Box component={Paper} sx={{boxSizing: "border-box",padding: "15px",borderRadius: "10px",marginY: "25px",}}>
            <Box sx={{ fontSize: "22px" }}>Ostatni pomiar</Box>
            <Stack sx={{ marginTop: "10px" }} direction="row" justifyContent="space-around" alignItems="center" alignContent="center">
              <Box sx={{ textAlign: "center" }}>
                  {new Date(data[0]?.createdAt).toLocaleString('pl-PL', {day: "2-digit", month: 'numeric', year: 'numeric', hour: 'numeric', minute: 'numeric', second: '2-digit'})}
              </Box>
              <Box sx={{ fontSize: "46px" }}>
                  {roundTemp(data[0]?.tempValue) + "º"}
              </Box>
            </Stack>
          </Box>
          <TableContainer sx={{ borderRadius: "10px", marginBottom: "25px" }} component={Paper}>
            <Table aria-label="custom pagination table">
              <TableHead>
                <TableRow>
                  <TableCell align="center">Czas pomiaru</TableCell>
                  <TableCell align="center">Wartość temperatury</TableCell>
                </TableRow>
              </TableHead>
              <TableBody>
                {(rowsPerPage > 0
                  ? data.slice(
                      page * rowsPerPage,
                      page * rowsPerPage + rowsPerPage
                    )
                  : data
                ).map((data) => (
                  <TableRow key={data._id}>
                    <TableCell align="center">
                      {new Date(data?.createdAt).toLocaleString('pl-PL', {day: "2-digit", month: 'numeric', year: 'numeric', hour: 'numeric', minute: 'numeric', second: '2-digit'})}
                    </TableCell>
                    <TableCell align="center">
                      {roundTemp(data?.tempValue) + "º"}
                    </TableCell>
                  </TableRow>
                ))}

                {emptyRows > 0 && (
                  <TableRow style={{ height: 53 * emptyRows }}>
                    <TableCell colSpan={6} />
                  </TableRow>
                )}
              </TableBody>
              <TableFooter>
                <TableRow>
                  <TablePagination
                    colSpan={3}
                    count={data.length}
                    rowsPerPage={rowsPerPage}
                    page={page}
                    onPageChange={handleChangePage}
                    ActionsComponent={TablePaginationActions}
                    rowsPerPageOptions={[]}
                  />
                </TableRow>
              </TableFooter>
            </Table>
          </TableContainer>
          </Stack>
      </Box>
    </ThemeProvider>
  );
}

export default App;
