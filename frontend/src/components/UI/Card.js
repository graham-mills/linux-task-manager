import classes from "./Card.module.css";

const Card = props => {
    return (
        <section className={classes.card}>
            <div className={classes.header}>
                <h4>{props.title}</h4>
            </div>
            <div className={classes.content}>
                {props.children}
            </div>
            <div className={classes.footer}/>
        </section>
    );
};

export default Card;