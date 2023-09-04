import classes from "./ProcessList.module.css";
import { useState } from "react";

const EmptyRow = () => (
   <>
      <tr>
         <td />
         <td />
         <td />
         <td />
         <td />
      </tr>
   </>
);

const TableHeader = ({
   label,
   sortName,
   selectedForSorting,
   handleSort,
   sortDirection,
}) => {
   return (
      <th
         className={classes.sortable}
         onClick={() => {
            handleSort(sortName);
         }}
      >
         {label}
         {selectedForSorting && sortDirection && <span>&#8595;</span>}
         {selectedForSorting && !sortDirection && <span>&#8593;</span>}
      </th>
   );
};

const ListTable = ({ processes }) => {
   const [sortColumn, setSortColumn] = useState("cpu_usage_percent");
   const [sortAscending, setSortAscending] = useState(false);

   const handleSort = (columnSortName) => {
      if (sortColumn === columnSortName) {
         // Toggle sort direction
         setSortAscending((sortDirection) => !sortDirection);
         return;
      } else {
         // Change column to sort by
         setSortColumn(columnSortName);
      }
   };

   let sortedProcs = [...processes];
   sortedProcs.sort((a, b) => {
      if (a[sortColumn] > b[sortColumn]) {
         return sortAscending ? 1 : -1;
      }
      if (a[sortColumn] < b[sortColumn]) {
         return sortAscending ? -1 : 1;
      }
      return 0;
   });
   const tableRows =
      sortedProcs.length > 0 ? (
         sortedProcs.map((proc) => (
            <tr key={proc.pid} title={proc.command}>
               <td>{proc.pid}</td>
               <td>{proc.ppid}</td>
               <td>{proc.name}</td>
               <td>{proc.cpu_usage_percent.toFixed(1)}%</td>
               <td>{proc.mem_usage_percent.toFixed(1)}%</td>
            </tr>
         ))
      ) : (
         <EmptyRow />
      );

   const tableHeaderRow = (
      <tr>
         <TableHeader
            label="PID"
            sortName="pid"
            selectedForSorting={sortColumn === "pid"}
            sortDirection={sortAscending}
            handleSort={handleSort}
         />
         <TableHeader
            label="PPID"
            sortName="ppid"
            selectedForSorting={sortColumn === "ppid"}
            sortDirection={sortAscending}
            handleSort={handleSort}
         />
         <TableHeader
            label="Name"
            sortName="name"
            selectedForSorting={sortColumn === "name"}
            sortDirection={sortAscending}
            handleSort={handleSort}
         />
         <TableHeader
            label="CPU"
            sortName="cpu_usage_percent"
            selectedForSorting={sortColumn === "cpu_usage_percent"}
            sortDirection={sortAscending}
            handleSort={handleSort}
         />
         <TableHeader
            label="Mem"
            sortName="mem_usage_percent"
            selectedForSorting={sortColumn === "mem_usage_percent"}
            sortDirection={sortAscending}
            handleSort={handleSort}
         />
      </tr>
   );

   return (
      <table className={classes["proc-table"]}>
         <thead>{tableHeaderRow}</thead>
         <tbody>{tableRows}</tbody>
      </table>
   );
};

export default ListTable;
