import classes from "./ProcessList.module.css";
import { useCallback, useState } from "react";

// Component to display processes in a table with sortable columns,
// clicking a header cell should enable sorting processes for that column,
// clicking a header cell already selected for sorting should toggle sort
// direction
const ListTable = ({ processes }) => {
   const [sortProperty, setSortProperty] = useState("cpu_usage_percent");
   const [sortAscending, setSortAscending] = useState(false);

   const handleSort = useCallback(
      (columnSortName) => {
         if (sortProperty === columnSortName) {
            // Toggle sort direction
            setSortAscending((sortDirection) => !sortDirection);
         } else {
            // Change column to sort by
            setSortProperty(columnSortName);
         }
      },
      [sortProperty, setSortAscending, setSortProperty]
   );

   const sortedProcs = sortProcsByProperty(
      processes,
      sortProperty,
      sortAscending
   );
   const tableRows = sortedProcs.map((proc) => (
      <TableProcessRow key={proc.pid} proc={proc} />
   ));

   return (
      <table className={classes["proc-table"]}>
         <thead>
            <TableHeaderRow
               sortProperty={sortProperty}
               sortAscending={sortAscending}
               handleSort={handleSort}
            />
         </thead>
         <tbody>{tableRows.length > 0 ? tableRows : <EmptyRow />}</tbody>
      </table>
   );
};

export default ListTable;

// Empty/placeholder <tr> element to be displayed
// when the table has no data to display
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

const TableHeaderRow = ({ sortProperty, sortAscending, handleSort }) => (
   <tr>
      <TableHeader
         label="PID"
         sortName="pid"
         selectedForSorting={sortProperty === "pid"}
         sortDirection={sortAscending}
         handleSort={handleSort}
      />
      <TableHeader
         label="PPID"
         sortName="ppid"
         selectedForSorting={sortProperty === "ppid"}
         sortDirection={sortAscending}
         handleSort={handleSort}
      />
      <TableHeader
         label="Name"
         sortName="name"
         selectedForSorting={sortProperty === "name"}
         sortDirection={sortAscending}
         handleSort={handleSort}
      />
      <TableHeader
         label="CPU"
         sortName="cpu_usage_percent"
         selectedForSorting={sortProperty === "cpu_usage_percent"}
         sortDirection={sortAscending}
         handleSort={handleSort}
      />
      <TableHeader
         label="Mem"
         sortName="mem_usage_percent"
         selectedForSorting={sortProperty === "mem_usage_percent"}
         sortDirection={sortAscending}
         handleSort={handleSort}
      />
   </tr>
);

// Clickable <th> element
// Arguments:
// - label: Displayed header text
// - sortName: The property name or ID passed back to the sort handler
// - handleSort: Sort handler callback
// - sortDirection:
const TableHeader = ({
   label,
   sortName,
   selectedForSorting,
   handleSort,
   sortDirection,
}) => (
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

// <tr> element to display process information
const TableProcessRow = ({ proc }) => (
   <tr title={proc.command}>
      <td>{proc.pid}</td>
      <td>{proc.ppid}</td>
      <td>{proc.name}</td>
      <td>{proc.cpu_usage_percent.toFixed(1)}%</td>
      <td>{proc.mem_usage_percent.toFixed(1)}%</td>
   </tr>
);

// Returns a list of procs sorted by an object property
const sortProcsByProperty = (procs, sortProperty, sortAscending) => {
   let sortedProcs = [...procs];
   sortedProcs.sort((a, b) => {
      if (a[sortProperty] > b[sortProperty]) {
         return sortAscending ? 1 : -1;
      }
      if (a[sortProperty] < b[sortProperty]) {
         return sortAscending ? -1 : 1;
      }
      return 0;
   });
   return sortedProcs;
};
