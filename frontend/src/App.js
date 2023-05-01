import { Container, Stack, Box, TableContainer, Table, Paper, TableHead, TableRow, TableCell, TableBody } from '@mui/material';
import {useState, useEffect} from 'react'
import axios from 'axios';

function App() {
  const [data, setData] = useState([]);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null)

  useEffect(() => {
    async function fetchData(){
      try{
        setLoading(true)
        let res = await axios.get('http://frog01-20917.wykr.es/');
        let temperatures = res.data;
        setData(temperatures);
      }catch(error){
        setLoading(false)
        setError('An error occurred. Awkward..')
      }}
      fetchData();
    },[])
  return (
    <Box sx={{bgcolor: '#303030'}}>
    <Container>
        <Stack direction="column"
              justifyContent="center"
              alignItems="center" >
            <Box>
              <Stack direction="row"
              justifyContent="center"
              alignItems="center" >
                  <Box> 
                      Ostatni pomiar
                      {new Date(data[0].createdAt).toLocaleString()}
                  </Box>
                  <Box>
                      {data[0].tempValue + "º"}
                  </Box>
              </Stack>
            </Box>
      <TableContainer sx={{bgcolor: '#424242', color: '#fff !important', maxWidth: '400px'}} component={Paper}>
      <Table sx={{color: '#fff'}}>
        <TableHead>
          <TableRow >
            <TableCell sx={{color: '#fff !important'}} align="left">Czas pomiaru</TableCell>
            <TableCell sx={{color: '#fff !important'}} align="center">Wartość pomiaru</TableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          {data.map((row, index) => {
            if(index!=0)
              return(<TableRow
                key={row._id}
                sx={{ '&:last-child td, &:last-child th': { border: 0 } }}
              >
                <TableCell sx={{color: '#fff !important'}} align="left" component="th" scope="row">
                  {new Date(row.createdAt).toLocaleString()}</TableCell>
                <TableCell sx={{color: '#fff !important'}} align="center">{row.tempValue + "º"}</TableCell>
              </TableRow>
            )
          })}
        </TableBody>
      </Table>
    </TableContainer>
        </Stack>
    </Container>
    </Box>
  );
}

export default App;
